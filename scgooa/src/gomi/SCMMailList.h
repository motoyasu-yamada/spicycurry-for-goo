    //   表示中メール一覧のメールIDリスト 
    private static int[]  MAIL_INDEX_LIST;
    //   現在閲覧しているメール 
    private static int    CURRENT_MAIL_INDEX;
    /**
     *  メール一覧の選択状態
     */
    private static int[] _mailListState = new int[2];
    /**
     *  メール一覧の行高
     */
    private static int _mailList1LineHeight;
    /**
     *  フォルダ内のメール
     */
    private static Vector _mailList;

    /**
     *  メール一覧を開く.
     */
    private static void openMailList(int folder) {
        CURRENT_FOLDER = folder;
        ASSERT(folder == FLD_NEW   ||
               folder == FLD_READ  ||
               folder == FLD_SEND  ||
               folder == FLD_DRAFT ||
               folder == FLD_SAVE);

        _mailListState[LIST_STATE_SELECT]   = 0;
        _mailListState[LIST_STATE_DRAW_START] = 0;
        return2MailList();
    }

    /**
     *  メール一覧に戻る.
     */
    private static void return2MailList() {
        ASSERT(_currentScene != SCENE_MAIL_LIST);
       _mailList  = new Vector();
        int count = 0;
        Integer folderDraft   = new Integer(FLD_DRAFT);
        Integer folderSend    = new Integer(FLD_SEND);
        Integer folderCurrent = new Integer(CURRENT_FOLDER);
        Integer accountId     = new Integer(ACTIVE_ACCOUNT_ID);

        MAIL_INDEX_LIST = new int[ML_MAIL.size()];
        for(int i = ML_MAIL.size()-1;i >= 0;i--) {
            Integer id = (Integer)ML_FOLDERID.elementAt(i);
            ASSERT(id != null);
            if (!id.equals(folderCurrent) || !ML_ACCOUNTID.elementAt(i).equals(accountId)) {
                continue;
            }
            String[] item = new String[]{
                (String)ML_MAIL.elementAt(i),
                (String)ML_SUBJECT.elementAt(i),
                (String)ML_DATE.elementAt(i),
            };
            _mailList.addElement(item);
            MAIL_INDEX_LIST[count++]=i;
        }

        if (count == 0) {
            showDialog(GETMSTR(IDS_FOLDER_EMPTY),Dialog.DIALOG_INFO);
            openFolderList();
            return;
        }

        String title = GETMSTR(IDS_MAILBOX_GET);
        if(CURRENT_FOLDER==FLD_READ) {
            title = GETMSTR(IDS_MAILBOX_READ);
        }
        if(CURRENT_FOLDER==FLD_SEND) {
            title = GETMSTR(IDS_MAILBOX_SEND);
        }
        if(CURRENT_FOLDER==FLD_DRAFT) {
            title = GETMSTR(IDS_MAILBOX_DRAFT);
        }
        if(CURRENT_FOLDER==FLD_SAVE) {
            title = GETMSTR(IDS_MAILBOX_SAVE);
        }
        title += GETMSTR(IDS_MAIL) + GETMSTR(IDS_VIEW);

        _mailList1LineHeight = Math.max(_font.getHeight(),IMAGE_MAIL_ICON_HEIGHT) + 4;
        initList(_mailListState,_mailList1LineHeight * 2,_mailList.size(),title);

        _currentScene     = SCENE_MAIL_LIST;
        startTimer();
        if(CURRENT_FOLDER == FLD_NEW || CURRENT_FOLDER == FLD_READ) {
            activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_SFK_CLEAR));
        } else {
            activateCanvas(GETMSTR(IDS_SFK_RETURN),null);
        }
    }

    /**
        メール一覧のキーイベント処理
        @see Canvas.processEvent
    */
    private static void processMailList(int key) {
        if (key == Display.KEY_DOWN || key == Display.KEY_UP) {
            startTimer();
        }
        key = processList(key);
        if (key == EVENT_NULL) {
            return;
        }
        if (key == Display.KEY_SOFT1) {
            openFolderList();
            return;
        }

        if (key == Display.KEY_SOFT2) {
            if( ! (CURRENT_FOLDER == FLD_NEW || CURRENT_FOLDER == FLD_READ)) {
                return;
            } 
            if(showDialog(GETMSTR(IDS_FOLDER_CLEAR_CONFIRM),Dialog.DIALOG_YESNO)
               == Dialog.BUTTON_YES) {
                int n = 0;
                while(n < ML_MAIL.size()) {
                    Object id;
                    id = ML_FOLDERID.elementAt(n);
                    if(id.equals(new Integer(CURRENT_FOLDER))) {
                        removeML(n);
                    } else {
                        n++;
                    }
                }
                saveMail();
                showDialog(GETMSTR(IDS_FOLDER_CLEAR_DONE),Dialog.DIALOG_INFO);
                openFolderList();
            }
            return;
        }

        if (key != Display.KEY_SELECT) {
            return;
        }

        CURRENT_MAIL_INDEX = _listState[LIST_STATE_SELECT];
        if (!openMailBody(CURRENT_MAIL_INDEX)) {
            showDialog(GETMSTR(IDS_CAN_NOT_DOWNLOAD_BODY),Dialog.DIALOG_ERROR);
        }
    }

    /**
     *  メール一覧の各行の描画.
     */
    private static void paintMailListItem(Graphics g,int i,int y,boolean select) {
        String[] item = (String[])_mailList.elementAt(i);

        //  二段目の行のY座標位置
        int y2 = y + _mailList1LineHeight;
        int x  = 2 + IMAGE_MAIL_ICON_WIDTH + ICON_TEXT_MARGIN;
        String subject = padding(Integer.toString(i+1),3,' ') + ":" + item[1];
        drawText(g,item[0],x, y,_canvas.getWidth() - x,_mailList1LineHeight,2,false,ALIGN_LEFT);
        if (select) {
            subject += " / " + item[2];
            drawTicker(g,subject,getTicker(),x,y2,_canvas.getWidth() - x,_mailList1LineHeight,2);
        } else {
            drawText(g,subject,x, y2,_canvas.getWidth() - x,_mailList1LineHeight,2,false,ALIGN_LEFT);
        }
        int icon = IMAGE_MAIL_ICON_WIDTH;
        if(CURRENT_FOLDER == FLD_NEW ||
           CURRENT_FOLDER == FLD_READ ||
           CURRENT_FOLDER == FLD_SAVE) {
            icon *= IMAGE_MAIL_ICON_INDEX_FROM;
        }
        if(CURRENT_FOLDER == FLD_SEND ||
           CURRENT_FOLDER == FLD_DRAFT) {
            icon *= IMAGE_MAIL_ICON_INDEX_TO;
        }
        g.drawImage(MRIMAGES[MIMAGE_MAIL],
                    2,
                    y + 2,
                    icon,
                    0,
                    IMAGE_MAIL_ICON_WIDTH,
                    IMAGE_MAIL_ICON_HEIGHT);
        g.drawImage(MRIMAGES[MIMAGE_MAIL],
                    2,
                    y + 2 + _mailList1LineHeight,
                    IMAGE_MAIL_ICON_INDEX_SUBJECT * IMAGE_MAIL_ICON_WIDTH,
                    0,
                    IMAGE_MAIL_ICON_WIDTH,
                    IMAGE_MAIL_ICON_HEIGHT);
    }
