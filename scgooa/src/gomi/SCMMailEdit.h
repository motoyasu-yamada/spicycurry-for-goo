
    /**
     *  編集画面上のテキストボックス
     */
    private static TextBox[] _mailEditTextBoxs;
    /**
     *  編集画面上のボタン
     */
    private static Button[] _mailEditButtons;
    /**
     *  現在編集中のアドレスボックス
     */
    private static int _mailEditCurrentTextBox;
    /**
     *  添付画像一覧
     */
    private static ListBox _mailEditImageList;
    /**
     *  戻り先画面
     */
    private static int _mailEditReturnScene;
    /**
     *  メール編集画面
     */
    private static Panel _mailEditPanel;


    /**
    *   メール送信フォームを開く
    *   @param mailto   メールあて先
    *   @param subject  件名
    *   @param body     本文
    */
    private static void openMailEdit(
        int     returnScene,
        String  mailto,
        String  subject,
        String  body,
        int attachFileId) {
        ASSERT(returnScene == SCENE_MAIN         ||
               returnScene == SCENE_MAIL_LIST    ||
               returnScene == SCENE_FOLDER_LIST  ||
               returnScene == SCENE_MAIL_BODY    ||
               returnScene == SCENE_ADDRESS_LIST ||
               returnScene == SCENE_BROWSER      ||
               returnScene == SCENE_IMG_FOLDER   ||
               returnScene == SCENE_ICON_FOLDER);

        _mailEditReturnScene = returnScene;

        // アカウントが設定されていない時は警告する
        if(ACTIVE_ACCOUNT_ID == INVALID_ACCOUNT_ID) {
            _currentScene = -1;
            showDialog(GETMSTR(IDS_ACCOUNT_IS_NOT_SELECTED),Dialog.DIALOG_ERROR);
            exitMailEdit();
            return;
        }

        ASSERT(MAIL_BUTTON_TO  == MAIL_TEXTBOX_TO);
        ASSERT(MAIL_BUTTON_CC  == MAIL_TEXTBOX_CC);
        ASSERT(MAIL_BUTTON_BCC == MAIL_TEXTBOX_BCC);

        Label   label;
        Panel   panel;
        Button  button;
        TextBox textbox;
        int     color;
        int     panelWidth;
        
        ASSERT(_mailEditPanel == null);
        ASSERT(_mailEditButtons    == null);
        ASSERT(_mailEditTextBoxs   == null);
        _mailEditCurrentTextBox = 0;
        color           = Graphics.getColorOfRGB(0x99,0xcc,0xff);
        _mailEditPanel =
        panel           = new Panel();
        // panel.setFont(_font);
        _mailEditTextBoxs   = new TextBox[MAIL_TEXTBOX_NUM];
        _mailEditButtons    = new Button[MAIL_BUTTON_NUM];
        
        panelWidth    = panel.getWidth() - PANEL_SCREEN_MARGIN;
        panel.setTitle(GETMSTR(IDS_TIT_NEWMAIL));
        panel.setBackground(color);

        //  宛先
        label = new Label(GETMSTR(IDS_LBL_TO),Label.LEFT);
        label.setBackground(color);
        panel.add(label);

        button  = new Button(EMOJI_ADDRESS_BOOK);
        textbox = new TextBox(mailto, 30, 1, TextBox.DISPLAY_ANY);
        textbox.setSize(panelWidth - label.getWidth() - button.getWidth(),textbox.getHeight());
        label.  setBackground(color);
        textbox.setInputMode(TextBox.ALPHA);
        panel.add(textbox);
        panel.add(button);
        textbox.requestFocus();
        _mailEditTextBoxs[MAIL_TEXTBOX_TO] = textbox;
        _mailEditButtons [MAIL_BUTTON_TO]  = button;

        //  件名
        label = new Label(GETMSTR(IDS_LBL_SUBJECT),Label.LEFT);
        textbox = new TextBox(subject, 30, 1, TextBox.DISPLAY_ANY);
        textbox.setSize(panelWidth - label.getWidth(),textbox.getHeight());
        label.  setBackground(color);
        textbox.setInputMode(TextBox.KANA);
        panel.add(label);
        panel.add(textbox);
        _mailEditTextBoxs[MAIL_TEXTBOX_SUBJECT] = textbox;

        // 本文
        label   = new Label(GETMSTR(IDS_LBL_BODY),Label.LEFT);
        textbox = new TextBox(body,30,2,TextBox.DISPLAY_ANY);
        textbox.setSize(panelWidth - label.getWidth(),button.getHeight());
        label.  setBackground(color);
        textbox.setInputMode(TextBox.KANA);
        panel.add(label);
        panel.add(textbox);
        _mailEditTextBoxs[MAIL_TEXTBOX_BODY] = textbox;

        //  CC
        label   = new Label(GETMSTR(IDS_LBL_CC),Label.LEFT);
        button  = new Button(EMOJI_ADDRESS_BOOK);
        textbox = new TextBox("",30,1,TextBox.DISPLAY_ANY);
        textbox.setSize(panelWidth-label.getWidth() - button.getWidth(),textbox.getHeight());
        label.  setBackground(color);
        textbox.setInputMode(TextBox.ALPHA);
        panel.add(label);
        panel.add(textbox);
        panel.add(button);
        _mailEditButtons[MAIL_BUTTON_CC]   = button;
        _mailEditTextBoxs[MAIL_TEXTBOX_CC] = textbox;

        //  BCC
        label   = new Label(GETMSTR(IDS_LBL_BCC),Label.LEFT);
        button  = new Button(EMOJI_ADDRESS_BOOK);
        textbox = new TextBox("",30,1,TextBox.DISPLAY_ANY);
        textbox.setSize(panelWidth-label.getWidth() - button.getWidth(),textbox.getHeight());
        label.  setBackground(color);
        textbox.setInputMode(TextBox.ALPHA);
        panel.add(label);
        panel.add(textbox);
        panel.add(button);
        _mailEditButtons[MAIL_BUTTON_BCC]   = button;
        _mailEditTextBoxs[MAIL_TEXTBOX_BCC] = textbox;

        //  即送信/後で送信/下書/キャンセル
        button = new Button(GETMSTR(IDS_BTN_NOW_SEND));
        button.setSize(panelWidth / 2,button.getHeight());
        panel.add(button);
        _mailEditButtons[MAIL_BUTTON_SENDI] = button;

        button =new Button(GETMSTR(IDS_BTN_SEND));
        button.setSize(panelWidth / 2,button.getHeight());
        panel.add(button);
        _mailEditButtons[MAIL_BUTTON_SENDA]  = button;
        
        button =new Button(GETMSTR(IDS_BTN_DRAFT));
        button.setSize(panelWidth / 2,button.getHeight());
        panel.add(button);
        _mailEditButtons[MAIL_BUTTON_DRAFT]  = button;

        button =new Button(GETMSTR(IDS_BTN_CANCEL));
        button.setSize(panelWidth / 2,button.getHeight());
        panel.add(button);
        _mailEditButtons[MAIL_BUTTON_CANCEL] = button;

        //  添付ファイル一覧
        label = new Label(GETMSTR(IDS_LBL_ATTACH),Label.LEFT);
        label.setBackground(color);
        _mailEditImageList = new ListBox(ListBox.CHOICE,1);
        _mailEditImageList.setSize(panelWidth-label.getWidth(),_mailEditImageList.getHeight());
        panel.add(_mailEditImageList);

        int count   = FileSystem.mountDir(DIRI_IMG);
        _mailEditImageList.append(GETMSTR(IDS_NO_ATTACH));
        _mailEditImageList.select(0);
        for (int n = 0;n < count;n++) {
            int eid = FileSystem.index2Entry(n);
            _mailEditImageList.append(FileSystem.fileName(eid));
            if (attachFileId == eid) {
                _mailEditImageList.select(n+1);
            }
        }
        openMailEdit();
    }

    /**
    *   既にパネル作成済みのメール編集画面を開く
    */
    private static void openMailEdit() {
        ASSERT(_mailEditPanel != null);
        _currentScene = SCENE_MAIL_EDIT;
        _mailEditPanel.setComponentListener(_canvas);
        Display.setCurrent(_mailEditPanel);
        clearKeyEvent();
    }

    /**
    *   メール送信パネルのコンポーネントアクション
    */
    private static void componentAction_MailPanel(
        Component c,
        int       type) {

        String mailto;
        String subject;
        String body;
        String cc;
        String bcc;
        int    button;

        if(type != BUTTON_PRESSED) {
            return;
        }

        subject = _mailEditTextBoxs[MAIL_TEXTBOX_SUBJECT].getText();
        body    = _mailEditTextBoxs[MAIL_TEXTBOX_BODY]   .getText();
        mailto  = _mailEditTextBoxs[MAIL_TEXTBOX_TO] .getText();
        cc      = _mailEditTextBoxs[MAIL_TEXTBOX_CC] .getText();
        bcc     = _mailEditTextBoxs[MAIL_TEXTBOX_BCC].getText();
        for (button = 0;button < MAIL_BUTTON_NUM;button++) {
            if (c == _mailEditButtons[button]) {
                break;
            }
        }

        ASSERT(0 <= button && button < MAIL_BUTTON_NUM);

        if (button == MAIL_BUTTON_TO  ||
            button == MAIL_BUTTON_CC  ||
            button == MAIL_BUTTON_BCC ) {
            _mailEditCurrentTextBox = button;
            openAddressList(true);
            return;
        }

        if (button == MAIL_BUTTON_CANCEL) {
            exitMailEdit();
            return;
        }


        ASSERT(button == MAIL_BUTTON_SENDI ||
               button == MAIL_BUTTON_SENDA ||
               button == MAIL_BUTTON_DRAFT);

        if(getStringBytes(mailto + subject + body + cc + bcc).length
             > MAX_UPLOAD_LENGTH - 128) {
                showDialog(GETMSTR(IDS_MAILSIZE_OVER_LIMIT),Dialog.DIALOG_ERROR);
                return;
        }

        int attachid;
        int attachindex = _mailEditImageList.getSelectedIndex();
        if (attachindex <= 0) {
            attachid = -1;
        } else {
            int[] saved = FileSystem.curDir();
            FileSystem.mountDir(DIRI_IMG);
            attachid = FileSystem.index2Entry(attachindex-1);
            FileSystem.mountDir(saved);
        }

        //  後で送信 & 保存
        if(button == MAIL_BUTTON_SENDA ||
           button == MAIL_BUTTON_DRAFT ) {
            int msgConfirm;
            int msgDone;
            int folderid;
            msgConfirm = IDS_DRAFT_SAVE_CONFIRM;
            msgDone    = IDS_DRAFT_SAVE_DONE;
            folderid   = FLD_DRAFT;
            if(button == MAIL_BUTTON_SENDA) {
                msgConfirm = IDS_SENDA_SAVE_CONFIRM;
                msgDone    = IDS_SENDA_SAVE_DONE;
                folderid   = FLD_SEND;
            }
            if(showDialog(GETMSTR(msgConfirm),Dialog.DIALOG_YESNO)
                   == Dialog.BUTTON_NO) {
                return;
            }
            if(MAX_MAIL_ENTRY <= ML_MAIL.size()) {
                showDialog(GETMSTR(IDS_MAIL_OVER_LIMIT),Dialog.DIALOG_ERROR);
                return;
            }

            String date = format(Calendar.getInstance());
            ML_MAIL.     addElement(mailto);
            ML_SUBJECT.  addElement(subject);
            ML_BODY.     addElement(body);
            ML_DATE.     addElement(date);
            ML_ATTCH.    addElement(new String[0]);
            ML_IUIDL.    addElement("");
            ML_PGCOUNT.  addElement(new Integer(0));
            ML_FOLDERID. addElement(new Integer(folderid));
            ML_ACCOUNTID.addElement(new Integer(ACTIVE_ACCOUNT_ID));
            ML_SATTACH.addElement(new Integer(attachid));

            if (saveMail()) {
                showDialog(GETMSTR(msgDone),Dialog.DIALOG_INFO);
            }
        }
        //  すぐに送信
        if (button == MAIL_BUTTON_SENDI) {
            if (protocolSendMail(mailto,subject,body,cc,bcc,attachid)) {
                showDialog(GETMSTR(IDS_SENDI_SEND_DONE),
                           Dialog.DIALOG_INFO);
            } else {
                showDialog(GETMSTR(IDS_SENDI_SEND_ERR),
                           Dialog.DIALOG_ERROR);
                return;
            }
        }

        if(CURRENT_FOLDER==FLD_DRAFT || CURRENT_FOLDER==FLD_SEND) {
            removeML(MAIL_INDEX_LIST[CURRENT_MAIL_INDEX]);
            saveMail();
        }

        if (button == MAIL_BUTTON_SENDI ||
            button == MAIL_BUTTON_SENDA) {
            String[] addresses;
            int      start;
            int      end;
            int      count;
            int      index;
            addresses = new String[MAX_ADDRESS_ENTRY];
            count     = 0;
            start     = 0;
            index     = 0;

            String emails;
            emails = mailto  + ",";
            if (0 < cc.length()) {
                emails += cc  + ",";
            }
            if (0 < bcc.length()) {
                emails += bcc + ",";
            }
            TRACE("emails = " + emails);
            for(;;) {
                String email1;
                String name1;
                int    split1;
                int    split2;
                // 最大登録数をオーバーした
                if(count == MAX_ADDRESS_ENTRY) {
                    break;
                }
                // 終端チェック
                if (emails.length() <= start) {
                    ASSERT(start == emails.length());
                    break;
                }
                // セパレータ , を検索(絶対みつかる)
                TRACE("start = " + start);
                end = emails.indexOf(",",start);
                TRACE("end   = " + end  );
                ASSERT(end != -1);

                //  空のエントリは追加しない
                if (start == end) {
                    start  = end + 1;
                    continue;
                }

                // エントリを抽出する
                email1 = emails.substring(start,end);
                start  = end + 1;
                
                TRACE("email1 = " + email1);
                // メールアドレスと名前を切り分ける
                name1  = "";
                split1 = email1.indexOf("<");
                if(split1 != -1) {
                    split2 = email1.indexOf(">");
                    name1  = email1.substring (0,split1);
                    email1 = email1.substring (split1 + 1,split2);
                }
                TRACE("name1  = " + name1);
                TRACE("email1 = " + email1);
                // 重複エントリは登録しない
                // アドレス帳内の検索
                if (AD_MAIL.indexOf(email1) != -1) {
                    continue;
                }

                // 送信先からの検索
                boolean duplication = false;
                for (int find = 0;find < (index / 4);find++) {
                    if (addresses[find * 4].equals(email1)) {
                        duplication = true;
                        break;
                    }
                }
                if (duplication) {
                    continue;
                }
                // ユーザに登録するかどうか確認する
                if(showDialog(email1 + GETMSTR(IDS_SAVE_CONFIRM),Dialog.DIALOG_YESNO)
                    == Dialog.BUTTON_YES) {
                    addresses[index++] = email1;
                    addresses[index++] = name1;
                    addresses[index++] = "";
                    addresses[index++] = "";
                    count++;
                }
            }

            // アドレス帳登録画面を呼び出す
            if(0 < index) {

                TRACE("openAddressEdit");
                String[] addresses2;
                addresses2 = new String[index];
                System.arraycopy(addresses,
                                 0,
                                 addresses2,
                                 0,
                                 index);
                _mailEditPanel = null;
                _mailEditButtons    = null;
                _mailEditTextBoxs   = null;
                openAddressEdit(_mailEditReturnScene,
                                addresses2,
                                -1);
                return;
            }
        }

        exitMailEdit();
        return;
    }

    /**
    *   メール編集画面の終了
    *   戻り画面種別
    *       MAIN / FOLDER / MAIL_LIST(DRAFT) / ADDRESS_LIST / MAIL_BODY
    */
    private static void exitMailEdit() {
        _mailEditPanel = null;
        _mailEditButtons    = null;
        _mailEditTextBoxs   = null;
        _mailEditImageList  = null;

        int returnScene = _mailEditReturnScene;
        if (returnScene == SCENE_MAIN) {
            openMain();
            return;
        }
        if (returnScene == SCENE_FOLDER_LIST) {
            openFolderList();
            return;
        }
        if (returnScene == SCENE_MAIL_LIST) {
            return2MailList();
            return;
        }
        if (returnScene == SCENE_MAIL_BODY) {
            openMailBody();
            return;
        }
        if (returnScene == SCENE_ADDRESS_LIST) {
            openAddressList(false);
            return;
        }
        if (returnScene == SCENE_BROWSER) {
            returnBrowser();
            return;
        }
        if (returnScene == SCENE_IMG_FOLDER) {
            openImgFolder();
            return;
        }
        if (returnScene == SCENE_ICON_FOLDER) {
            openIconFolder();
            return;
        }
        ASSERT(false);
    }
