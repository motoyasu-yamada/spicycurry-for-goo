    private static boolean ADDRESS_LIST_CAN_SELECT;
    private static int[] _addressListState = new int[2];

    /**
    *   アドレス帳を開く
    *   @param selectAddress true  - メール作成時にアドレスを選択する場合
    *                        false - アドレス帳作成の場合
    */
    private static void openAddressList(boolean selectAddress) {
        ADDRESS_LIST_CAN_SELECT = selectAddress;
        int height = Math.max(_font.getHeight(),IMAGE_MAIL_ICON_HEIGHT) + 4;
        initList(_addressListState,height,AD_MAIL.size(),GETMSTR(IDS_ADD) + GETMSTR(IDS_VIEW));
        _currentScene = SCENE_ADDRESS_LIST;
        String softkey2 = selectAddress ? null : GETMSTR(IDS_SFK_MENU);
        activateCanvas(GETMSTR(IDS_SFK_RETURN),softkey2);
    }

    /**
        アドレス帳のアドレス一覧でのキー操作処理
    */
    private static void processAddressList(int key) {
        //ASSERT(param == Display.KEY_RELEASED_EVENT);
        ASSERT(Display.KEY_0 + 9 == Display.KEY_9);
        ASSERT(Display.KEY_9 + 1 == Display.KEY_ASTERISK);
        ASSERT(Display.KEY_9 + 2 == Display.KEY_POUND);

        key = processList(key);
        if (key == EVENT_NULL) {
            return;
        }

        // 前の画面に戻る
        if (key == Display.KEY_SOFT1) {
            if (ADDRESS_LIST_CAN_SELECT) {
                ASSERT(FLD_NEW <= CURRENT_FOLDER &&
                       CURRENT_FOLDER <= FLD_SAVE);
                openMailEdit();
                return;
            }
            openFolderList();
            return;
        }

        // ポップアップメニューを開く
        if (key == Display.KEY_SOFT2 && !ADDRESS_LIST_CAN_SELECT) {
            int menuid = (AD_MAIL.size() != 0) ? IDM_CONTEXT_ADDRESS_LIST : IDM_CONTEXT_ADDRESS_LIST_EMPTY;
            int select = trackMenu(null,loadStrings(MRESOURCES[menuid]),0,true);
            menuAddressList(select);
            return;
        }

        if (key != Display.KEY_SELECT) {
            return;
        }
        waitKeyRelease();

        if (!ADDRESS_LIST_CAN_SELECT) {
            return;
        }
        int select = _listState[LIST_STATE_SELECT];
        if(select == -1) {
            return;
        }

        int itemCount = AD_MAIL.size();
        if (itemCount == 0) {
            return;
        }
        ASSERT(0 <= select && select < itemCount);

        int currentTextBox = _mailEditCurrentTextBox;
        /*  選択したアドレスをテキストボックスに挿入する */
        if (currentTextBox == MAIL_BUTTON_TO ||
            currentTextBox == MAIL_BUTTON_CC ||
            currentTextBox == MAIL_BUTTON_BCC) {
            ASSERT(_mailEditTextBoxs != null);
            ASSERT(_mailEditTextBoxs[currentTextBox] != null);
            
            String mailto = _mailEditTextBoxs[currentTextBox].getText();
            ASSERT(mailto != null);
            if (mailto.length() != 0 ) {
                mailto += ",";
            }
            ASSERT(AD_MAIL != null);
            mailto += (String)AD_MAIL.elementAt(select);
            _mailEditTextBoxs[currentTextBox].setText(mailto);

            openMailEdit();
        }
    }

    /**
     *  アドレス帳一覧の各行の描画.
     */
    private static void paintAddressListItem(Graphics g,int i,int y) {
        String itemAddress = AD_NAME.elementAt(i) + " " + AD_MAIL.elementAt(i);
        int left = IMAGE_MAIL_ICON_WIDTH + 4;
        drawText(g,itemAddress,left,y,_canvas.getWidth() - left,_listLineHeight,2,false,ALIGN_LEFT);
        g.drawImage(MRIMAGES[MIMAGE_MAIL],
                    2,
                    y + (_listLineHeight - IMAGE_MAIL_ICON_HEIGHT) / 2,
                    IMAGE_MAIL_ICON_INDEX_NAME * IMAGE_MAIL_ICON_WIDTH,
                    0,
                    IMAGE_MAIL_ICON_WIDTH,
                    IMAGE_MAIL_ICON_HEIGHT);
    }

    /**
    *   アドレス帳一覧でのメニュー選択
    */
    
    private static void menuAddressList(int select) {
        if (AD_MAIL.size() == 0) {
            switch(select){
            case IDM_CONTEXT_ADDRESS_LIST_EMPTY_NEW:
                select = IDM_CONTEXT_ADDRESS_LIST_NEW;
                break;
            case IDM_CONTEXT_ADDRESS_LIST_EMPTY_IMPORT:
                select = IDM_CONTEXT_ADDRESS_LIST_IMPORT;
                break;
            }
        }
        
        int listSelect = _listState[LIST_STATE_SELECT];
        ASSERT(AD_MAIL.size() == 0 ||
               0 <= listSelect && listSelect < AD_MAIL.size());

        switch(select) {
        case IDM_CONTEXT_ADDRESS_LIST_NEW:
            openAddressEdit(SCENE_ADDRESS_LIST,
                            new String[]{"","","",""},
                            -1);
            return;
        case IDM_CONTEXT_ADDRESS_LIST_EDIT:
            String[] addresses = new String[]{
                                    (String)AD_MAIL.elementAt(listSelect),
                                    (String)AD_NAME.elementAt(listSelect),
                                    (String)AD_YOMI.elementAt(listSelect),
                                    (String)AD_TEL. elementAt(listSelect)};
            openAddressEdit(SCENE_ADDRESS_LIST,
                            addresses,
                            listSelect);
            return;
        case IDM_CONTEXT_ADDRESS_LIST_SEND:
            openMailEdit(SCENE_ADDRESS_LIST,
                         (String)AD_MAIL.elementAt(listSelect),
                         "",
                         "",
                         FILEID_INVALID);
            return;
        case IDM_CONTEXT_ADDRESS_LIST_DELETE:
            if(showDialog(GETMSTR(IDS_DEL_CONFIRM),
                          Dialog.DIALOG_YESNO)
               == Dialog.BUTTON_YES) {
                AD_MAIL.removeElementAt(listSelect);
                AD_NAME.removeElementAt(listSelect);
                AD_YOMI.removeElementAt(listSelect);
                AD_TEL. removeElementAt(listSelect);
                saveAddress();
                openAddressList(ADDRESS_LIST_CAN_SELECT);

                showDialog(GETMSTR(IDS_DEL_DONE),Dialog.DIALOG_INFO);
            }
            return;
        case IDM_CONTEXT_ADDRESS_LIST_IMPORT:
            if(showDialog(GETMSTR(IDS_ADDRESS_IMPORT_CONFIRM),
                          Dialog.DIALOG_YESNO)
               != Dialog.BUTTON_YES) {
               return;
            }
            if (!protocolImportAddress()) {
                showDialog(GETMSTR(IDS_ADDRESS_IMPORT_ERR),Dialog.DIALOG_INFO);
                return;
            }
            showDialog(GETMSTR(IDS_ADDRESS_IMPORT_DONE),Dialog.DIALOG_INFO);
            openAddressList(ADDRESS_LIST_CAN_SELECT);
            return;
        case IDM_CONTEXT_ADDRESS_LIST_EXPORT:
            if(showDialog(GETMSTR(IDS_ADDRESS_EXPORT_CONFIRM),
                          Dialog.DIALOG_YESNO) != Dialog.BUTTON_YES) {
                return;
            }
            if (!protocolExportAddress()) {
                showDialog(GETMSTR(IDS_ADDRESS_EXPORT_ERR),Dialog.DIALOG_INFO);
                return;
            }
            showDialog(GETMSTR(IDS_ADDRESS_EXPORT_DONE),Dialog.DIALOG_INFO);
            return;
        }
        ASSERT(false);
    }