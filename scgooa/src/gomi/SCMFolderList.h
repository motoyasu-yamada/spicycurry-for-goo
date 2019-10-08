    private static int[] _folderListState = new int[2];
    private static Vector _folderList;

    /**
    *   フォルダ一覧を開く
    */
    private static void openFolderList() {
        ASSERT(IDS_FOLDER_LIST_NEW == 0);
        Vector list            = new Vector();
        String[] names  = loadStrings(MRESOURCES[IDM_FOLDER_LIST]);
        ASSERT(names != null);
        Integer[] counts = new Integer[names.length];
        counts[IDS_FOLDER_LIST_NEW]     = calcFolderItem(FLD_NEW);
        counts[IDS_FOLDER_LIST_READ]    = calcFolderItem(FLD_READ);
        counts[IDS_FOLDER_LIST_CREATE]  = null;
        counts[IDS_FOLDER_LIST_SEND]    = calcFolderItem(FLD_SEND);
        counts[IDS_FOLDER_LIST_DRAFT]   = calcFolderItem(FLD_DRAFT);
        counts[IDS_FOLDER_LIST_SAVE]    = calcFolderItem(FLD_SAVE);
        counts[IDS_FOLDER_LIST_ADDRESS] = new Integer(AD_MAIL.size());
        counts[IDS_FOLDER_LIST_IMG]     = new Integer(FileSystem.mountDir(DIRI_IMG));
        counts[IDS_FOLDER_LIST_SND]     = new Integer(FileSystem.mountDir(DIRI_SND));
        counts[IDS_FOLDER_LIST_REQUEST] = null;
        for (int n = 0;n < names.length;n++) {
            ASSERT(names[n] != null);
            String line = names[n];
            if (counts[n] != null) {
                line += "(" + counts[n] + ")";
            }
            list.addElement(line);
        }

        String title = GETMSTR(IDS_NO_ACTIVE_ACCOUNT);
        for(int n = 0;n < ACCOUNT_ID_LIST.length;n++) {
            if(ACCOUNT_ID_LIST[n] == ACTIVE_ACCOUNT_ID) {
                title = ACCOUNT_NAME_LIST[n];
            }
        }
        ASSERT(title != null);

        _folderList = list;
        int lineHeight = Math.max(_font.getHeight(),IMAGE_MENU_ICON_SIZE) + 4;
        initList(_folderListState,lineHeight,_folderList.size(),title);
        _currentScene   = SCENE_FOLDER_LIST;
        activateCanvas(GETMSTR(IDS_SFK_RETURN),null);
    }

    /**
     *  フォルダ内に含まれるメール数を計算する
     *       フォルダを指定する番号
     *           FLD_NEW 〜 FLD_DRAFT
     */
    private static Integer calcFolderItem(int search) {
        Integer folderid;
        Integer accountId;
        int     itemCount;
        itemCount = 0;
        folderid  = new Integer(search);
        accountId = new Integer(ACTIVE_ACCOUNT_ID);
        for (int n=0;n < ML_MAIL.size();n++) {
            if(ML_FOLDERID.elementAt(n).equals(folderid) && 
               ML_ACCOUNTID.elementAt(n).equals(accountId)){
                itemCount++;
            }
        }
        return new Integer(itemCount);
    }

    /**
     *  フォルダ一覧のアイテムを描画する
     */
    private static void paintFolderListItem(Graphics g,int i,int y) {
        String item = (String)_folderList.elementAt(i);
        int left = IMAGE_MENU_ICON_SIZE + 4;
        drawText(g,item,left,y,_canvas.getWidth() - left,_listLineHeight,2,false,ALIGN_LEFT);
        g.drawImage(MRIMAGES[MIMAGE_MENU],
                    2,
                    y + (_listLineHeight - IMAGE_MENU_ICON_SIZE) / 2,
                    MIMAGE_MENU_ICON_FOLDER_LIST[i * 2],
                    MIMAGE_MENU_ICON_FOLDER_LIST[i * 2 + 1],
                    IMAGE_MENU_ICON_SIZE,
                    IMAGE_MENU_ICON_SIZE);
    }

    /**
     *  フォルダ一覧のキー処理
     */
    private static void processFolderList(int key) {
        key = processList(key);
        if (key == EVENT_NULL) {
            return;
        }
        if (key == Display.KEY_SOFT1) {
            openMain();
            return;
        }
        if (key != Display.KEY_SELECT) {
            return;
        }

        int selected = _listState[LIST_STATE_SELECT];

        //  新規メール作成
        if (selected == FLD_CREATE) {
            openMailEdit(SCENE_FOLDER_LIST,
                         "",
                         "",
                         "",
                         FILEID_INVALID);
            return;
        }

        //  アドレス帳を開く
        if (selected == FLD_ADDR) {
            openAddressList(false);
            return;
        }

        //  マイボックス待受を開く
        if (selected == FLD_IMG) {
            openImgFolder();
            return;
        }

        //  マイボックス音声を開く
        /* m_murai
        if (selected == FLD_SND) {
            openSndFolder();
            return;
        }
        */

        //  センター問合せを実施
        if (selected == FLD_DNLD) {
            trackRequest();
            return;
        }

        openMailList(selected);
    }

    /**
     *  メール要求をトラッキングする.
     */
    private final static void trackRequest() {
        if (ACTIVE_ACCOUNT_ID == INVALID_ACCOUNT_ID) {
            showDialog(GETMSTR(IDS_ACCOUNT_IS_NOT_SELECTED),Dialog.DIALOG_ERROR);
            return;
        }
        int select = trackMenu(null,loadStrings(MRESOURCES[IDM_POPUP_REQUEST]),0,true);
        if (select == -1) {
            return;
        }
        if (!protocolGetHeader(select == 1)) {
            return;
        }
        openMailList(FLD_NEW);
        return;
    }
