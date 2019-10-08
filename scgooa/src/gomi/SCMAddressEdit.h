//  $Id: SCMAddressEdit.h,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $

    /** アドレス帳編集画面を表示
            受信メールから差出人を登録
            新規アドレスを登録
            既存アドレスを編集
            送信時にアドレスを登録
    
        @see CURRENT_ADDRESS         呼び出す時に編集するアドレスの一覧をこの配列に登録
                                     各アドレス毎に メアド、名前、読み、電話番号
                                     配列の数 = アドレス数 * 4
        @see CURRENT_ADDRESS_INDEX   CURRENT_ADDRESS内の複数のアドレスの中で
                                     現在編集のアドレスのインデックス
        @see EDIT_ADDRESS_LIST_INDEX editIndex参照
        @param returnScene  編集後の戻りアドレス
        @param addressed    編集するアドレスの一覧の配列 CURRENT_ADDRESSの説明参照
        @param editIndex    新規登録の場合 -1
                            編集の場合     0 <= editIndex && editIndex < AD_MAIL.size();
        
    */
    private static int EDIT_ADDRESS_LIST_INDEX;
    private static void openAddressEdit(int returnscene,String[] addressed,int editIndex) {
        ASSERT(addressed != null);
        ASSERT((addressed.length % ADDRESS_ENTRY_NUM)  == 0);
        ASSERT(editIndex == -1 ||
               0 <= editIndex && editIndex < AD_MAIL.size());
        CURRENT_ADDRESS           = addressed;
        CURRENT_ADDRESS_INDEX     = 0;
        ADDRESS_EDIT_RETURN_SCENE = returnscene;
        _currentScene             = SCENE_ADDRESS_EDIT;
        EDIT_ADDRESS_LIST_INDEX   = editIndex;
        openAddressEdit();
    }

    private static void openAddressEdit() {
        Button   button;
        String[] address;
        Panel    panel;
        Label    label;
        int      color;
        TextBox  textbox;
        int      paelWidth;
        ASSERT(CURRENT_ADDRESS != null);
        ASSERT(0 < ADDRESS_ENTRY_NUM);
        ASSERT(0 <= CURRENT_ADDRESS_INDEX);
        ASSERT(_currentScene == SCENE_ADDRESS_EDIT);

        address = new String[ADDRESS_ENTRY_NUM];
        System.arraycopy(CURRENT_ADDRESS,
                         CURRENT_ADDRESS_INDEX * ADDRESS_ENTRY_NUM,
                         address,
                         0,
                         ADDRESS_ENTRY_NUM);
        ADDRESS_TEXTBOXS = new TextBox[4];
        ADDRESS_BUTTONS  = new Button[2];

        color         = Graphics.getColorOfRGB(0x99,0xcc,0xff);

        panel         = new Panel();
        // panel.setFont(_font);
        paelWidth     = panel.getWidth() - PANEL_SCREEN_MARGIN;
        panel.setBackground(color);
        panel.setTitle(GETMSTR(IDS_ADD)+GETMSTR(IDS_BTN_REG));

        // メールアドレス
        label = new Label(GETMSTR(IDS_LBL_MAIL)+"  "+GETMSTR(IDS_LBL_ESS),Label.LEFT);
        label.setSize(paelWidth,label.getHeight());
        label.setBackground(color);
        panel.add(label);
        textbox = new TextBox(address[ADDRESS_EMAIL], 30,1,TextBox.DISPLAY_ANY);
        textbox.setSize(paelWidth,textbox.getHeight());
        textbox.setInputMode(TextBox.ALPHA);
        panel.add(textbox);
        ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_EMAIL] = textbox;

        // 名前
        label =new Label(GETMSTR(IDS_LBL_NAME)+"      "+GETMSTR(IDS_LBL_ESS),
                         Label.LEFT);
        label.setSize(paelWidth,label.getHeight());
        label.setBackground(color);
        panel.add(label);
        textbox = new TextBox(address[ADDRESS_NAME],
                              30,
                              1,
                              TextBox.DISPLAY_ANY);
        textbox.setSize(paelWidth,textbox.getHeight());
        textbox.setInputMode(TextBox.KANA);
        panel.add(textbox);
        ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_NAME] = textbox;

        // 読み
        label=new Label(GETMSTR(IDS_LBL_YOMI),Label.LEFT);
        label.setSize(paelWidth,label.getHeight());
        label.setBackground(color);
        panel.add(label);
        textbox = new TextBox(address[ADDRESS_YOMI],
                              30,
                              1,
                              TextBox.DISPLAY_ANY);
        textbox.setSize(paelWidth,textbox.getHeight());
        textbox.setInputMode(TextBox.KANA);
        panel.add(textbox);
        ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_YOMI] = textbox;

        // 電話番号
        label=new Label(GETMSTR(IDS_LBL_TEL),Label.LEFT);
        label.setSize(paelWidth,label.getHeight());
        label.setBackground(color);
        panel.add(label);
        textbox = new TextBox(address[ADDRESS_TEL],
                              30,
                              1,
                              TextBox.DISPLAY_ANY);
        textbox.setSize(paelWidth,textbox.getHeight());
        textbox.setInputMode(TextBox.NUMBER);
        panel.add(textbox);
        ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_TEL] = textbox;

        // 登録ボタン
        button = new Button(GETMSTR(IDS_BTN_REG));
        button.setSize(paelWidth,button.getHeight());
        panel.add(button);
        ADDRESS_BUTTONS[ADDRESS_BUTTON_OK] = button;

        // キャンセルボタン
        button = new Button(GETMSTR(IDS_SFK_RETURN));
        button.setSize(paelWidth,button.getHeight());
        panel.add(button);
        ADDRESS_BUTTONS[ADDRESS_BUTTON_CANCEL] = button;

        panel.setComponentListener(_canvas);
        Display.setCurrent(panel);

        clearKeyEvent();
    }


    /** アドレス帳パネルのコンポーネントアクション処理
        戻先:   メール送信画面
                メール本文
                アドレス帳
        @see componentAction
    */
    private static void componentAction_AddressPanel(
        Component c,
        int type) {
        String addresses;
        String email;
        String name;
        String yomi;
        String tel;
        int    index;
        int    length;
        if(type != BUTTON_PRESSED) {
            return;
        }
        // 登録キャンセル
        if (c  == ADDRESS_BUTTONS[ADDRESS_BUTTON_CANCEL]) {
            exitAddressEdit();
            return;
        }
        ASSERT(c  == ADDRESS_BUTTONS[ADDRESS_BUTTON_OK]);

        //  編集データの正当性を検査
        ASSERT((CURRENT_ADDRESS.length % ADDRESS_ENTRY_NUM)  == 0);
        ASSERT(((CURRENT_ADDRESS_INDEX + 1)* ADDRESS_ENTRY_NUM) <= CURRENT_ADDRESS.length);
        
        email = ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_EMAIL].getText().trim();
        name  = ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_NAME ].getText().trim();
        yomi  = ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_YOMI ].getText().trim();
        tel   = ADDRESS_TEXTBOXS[ADDRESS_TEXTBOX_TEL  ].getText().trim();

        length = getStringBytes(email).length;
        if(length == 0) {
            showDialog(GETMSTR(IDS_EMPTY_REC_MAIL),
                       Dialog.DIALOG_ERROR);
            return;
        }
        // メールアドレスの最大文字数と入力必須
        if(MAX_ADDRESS_MAIL_LENGTH < length) {
            showDialog(GETMSTR(IDS_ADDRESS_LEN_EXCEED),
                       Dialog.DIALOG_ERROR);
            return;
        }

        // メールアドレスの入力チェック
        if(!checkString(CHECK_STRING_MAIL,email))      {
            showDialog(GETMSTR(IDS_ADDRESS_WRONG_INPUT),
                       Dialog.DIALOG_ERROR);
            return;
        }

        // 名前の最大文字数と入力必須
        length = getStringBytes(name).length;
        if(length == 0) {
            showDialog(GETMSTR(IDS_EMPTY_REC_NAME),
                       Dialog.DIALOG_ERROR);
            return;
        }
        if(MAX_ADDRESS_NAME_LENGTH < length) {
            showDialog(GETMSTR(IDS_NAME_LEN_EXCEED),
                       Dialog.DIALOG_ERROR);
            return;
        }

        // 読みの文字数
        length = getStringBytes(yomi).length;
        if(MAX_ADDRESS_YOMI_LENGTH < length) {
            showDialog(GETMSTR(IDS_YOMI_LEN_EXCEED),
                       Dialog.DIALOG_ERROR);
            return;
        }
        if(!checkString(CHECK_STRING_YOMI,yomi))      {
            showDialog(GETMSTR(IDS_YOMI_RULE),
                       Dialog.DIALOG_ERROR);
            return;
        }

        // 電話番号の文字数をチェック
        length = getStringBytes(tel).length;
        if(MAX_ADDRESS_TEL_LENGTH < length ||
           !checkString(CHECK_STRING_TELE,tel))      {
            showDialog(GETMSTR(IDS_TEL_LEN_EXCEED),
                       Dialog.DIALOG_ERROR);
            return;
        }

        //  編集中の場合は古いアドレスを削除
        TRACE("AD_MAI.size() = " + AD_MAIL.size());
        DEBUG_ONLY(validateAD());
        if (EDIT_ADDRESS_LIST_INDEX != -1) {
            ASSERT(0 <= EDIT_ADDRESS_LIST_INDEX &&
                   EDIT_ADDRESS_LIST_INDEX < AD_MAIL.size());
            AD_MAIL.removeElementAt(EDIT_ADDRESS_LIST_INDEX);
            AD_NAME.removeElementAt(EDIT_ADDRESS_LIST_INDEX);
            AD_YOMI.removeElementAt(EDIT_ADDRESS_LIST_INDEX);
            AD_TEL. removeElementAt(EDIT_ADDRESS_LIST_INDEX);
            
        }
        DEBUG_ONLY(validateAD());

        // 登録件数オーバーの為追加を中止
        if(MAX_ADDRESS_ENTRY <= AD_MAIL.size()) {
            showDialog(GETMSTR(IDS_ADDRESS_OVER_LIMIT),
                       Dialog.DIALOG_ERROR);
            exitAddressEdit();
            return;
        }
        
        //  アドレス帳にデータを登録
        AD_MAIL.addElement(email);
        AD_NAME.addElement(name);
        AD_YOMI.addElement(yomi);
        AD_TEL. addElement(tel);

        //  アドレス帳を読み順にソートする
        sortAddress();
        
        //  スクラッチパッドに保存する
        saveAddress();

        CURRENT_ADDRESS_INDEX++;
        // 追加登録があった場合
        if(CURRENT_ADDRESS_INDEX * ADDRESS_ENTRY_NUM < CURRENT_ADDRESS.length) {
            openAddressEdit();
            return;
        }
        ASSERT(CURRENT_ADDRESS.length ==
               CURRENT_ADDRESS_INDEX * ADDRESS_ENTRY_NUM);

        showDialog(GETMSTR(IDS_ADDRESS_SAVED),
                   Dialog.DIALOG_INFO);
        exitAddressEdit();
    }

    /**
    *   アドレス帳編集画面を終了する
    */
    private static void exitAddressEdit() {
        int returnScene;
        returnScene = ADDRESS_EDIT_RETURN_SCENE;
        DEBUG_ONLY(ADDRESS_EDIT_RETURN_SCENE = -1);
        TRACE("exitAddressEdit=" + returnScene);
        if (returnScene == SCENE_ADDRESS_LIST) {
            openAddressList(ADDRESS_LIST_CAN_SELECT);
            return;
        }
        if (returnScene == SCENE_MAIL_EDIT) {
            exitMailEdit();
            return;
        }
        if (returnScene == SCENE_MAIL_BODY) {
            openMailBody();
            return;
        }
        if (returnScene == SCENE_MAIL_LIST) {
            return2MailList();
            return;
        }
        if (returnScene == SCENE_MAIN) {
            openMain();
            return;
        }
        if (returnScene == SCENE_FOLDER_LIST) {
            openFolderList();
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

    /**
    *   アドレス帳を読み順でソートする
    *   ソート主キー : 読み(AD_YOMI)
    *   ソート副キー : 名前(AD_NAME)
    */
    private static void sortAddress() {
        DEBUG_ONLY(validateAD());
        Vector smail = new Vector();
        Vector sname = new Vector();
        Vector syomi = new Vector();
        Vector stele = new Vector();
        for (int n = 0;n < AD_YOMI.size();n++) {
            ASSERT(sname.size() == smail.size());
            ASSERT(syomi.size() == smail.size());
            ASSERT(stele.size() == smail.size());
            String yomi;
            String name;
            yomi = (String)AD_YOMI.elementAt(n);
            name = (String)AD_NAME.elementAt(n);
            int inspos = 0;
            while (inspos < syomi.size()) {
                if (yomi.compareTo((String)syomi.elementAt(inspos)) < 0) {
                    break;
                }
                inspos++;
            }
            ASSERT(0 <= inspos && inspos <= syomi.size());
            sname.insertElementAt(name,inspos);
            syomi.insertElementAt(yomi,inspos);
            smail.insertElementAt(AD_MAIL.elementAt(n),inspos);
            stele.insertElementAt(AD_TEL. elementAt(n),inspos);
        }
        ASSERT(sname.size() == smail.size());
        ASSERT(syomi.size() == smail.size());
        ASSERT(stele.size() == smail.size());
        AD_MAIL = smail;
        AD_NAME = sname;
        AD_YOMI = syomi;
        AD_TEL  = stele;
        DEBUG_ONLY(validateAD());
    }