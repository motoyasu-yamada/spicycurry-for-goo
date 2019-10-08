    /**
     *  選択されているGUI部品番号
     */
    private static int _gooSetPassportIDSelectedItem;
    /**
     *  GUI部品番号:パスポートID入力エリア
     */
    private static final int GOO_SET_PASSPORTID_ITEM_PASSPORTID = 0;
    /**
     *  GUI部品番号:パスポートパスワード
     */
    private static final int GOO_SET_PASSPORTID_ITEM_PASSWD = 1;
    /**
     *  GUI部品番号:OKボタン
     */
    private static final int GOO_SET_PASSPORTID_ITEM_OK         = 2;
    /**
     *  GUI部品番号:CANCELボタン
     */
    private static final int GOO_SET_PASSPORTID_ITEM_CANCEL     = 3;
    /**
     *  設定中のGooPassportID
     */
    private static String _gooSetPassportID;
    /**
     *  設定中のGooPassportPasswd
     */
    private static String _gooSetPassportPasswd;

    /**
     *  GooPassportID設定画面を開く
     */
    private static void openGooPassportID(int attachFileId) {
        loadConf();
        _gooSetPassportIDSelectedItem = GOO_SET_PASSPORTID_ITEM_PASSPORTID;
        _gooSetPassportID     = gooPassportID;
        _gooSetPassportPasswd = gooPassportPasswd;
        _currentScene         = SCENE_GOOPASSPORTID;
        activateCanvas(null,null);
    }

    /**
     *  パスポート設定画面を描画する.
     *
     */
    private static void paintSetGooPassportID(Graphics g) {
        int space = 2;
        int clientLeft    = space;
        int clientWidth   = _canvas.getWidth() - space * 2;
        int buttonWidth   = clientWidth / 2 - space;
        int fontHeight    = _font.getHeight();
        int buttonHeight  = fontHeight + 4;
        int textboxHeihgt = fontHeight + 4;
        int bgcolor;
        int fgcolor;

        //  背景消去
        setColorOfRGB(g,_colors[COLOR_BKGND]);
        g.fillRect(0,0,_canvas.getWidth(),_canvas.getHeight());
        //  タイトル描画
        int y = paintTitle(g,GETMSTR(IDS_GOOID_SETUP));
        y += space;

        //  入力エリア
        setColorOfRGB(g,_colors[COLOR_TEXT]);
        drawText(g,
                 GETMSTR(IDS_GOO_SET_PASSPORTID_PASSPORTID),
                 clientLeft,
                 y,
                 clientWidth,
                 textboxHeihgt,
                 space,
                 false,
                 ALIGN_LEFT);
        y += fontHeight + space;

        if (_gooSetPassportIDSelectedItem == GOO_SET_PASSPORTID_ITEM_PASSPORTID) {
            bgcolor = _colors[COLOR_SELECT_BKGND];
            fgcolor = _colors[COLOR_SELECT_TEXT];
        } else {
            bgcolor = _colors[COLOR_CLIENT_BKGND];
            fgcolor = _colors[COLOR_CLIENT_TEXT];
        }
        drawBox(g,clientLeft,y,clientWidth,textboxHeihgt,bgcolor,_colors[COLOR_FRAME]);
        setColorOfRGB(g,fgcolor);
        drawText(g,_gooSetPassportID,clientLeft,y,clientWidth,textboxHeihgt,space,false,ALIGN_LEFT);
        y += textboxHeihgt + space;

        //  パスポートパスワード
        setColorOfRGB(g,_colors[COLOR_TEXT]);
        drawText(g,
                 GETMSTR(IDS_GOO_SET_PASSPORTID_PASSPORTPASSWD),
                 clientLeft,
                 y,
                 clientWidth,
                 textboxHeihgt,
                 space,
                 false,
                 ALIGN_LEFT);
        y += fontHeight + space;

        if (_gooSetPassportIDSelectedItem == GOO_SET_PASSPORTID_ITEM_PASSWD) {
            bgcolor = _colors[COLOR_SELECT_BKGND];
            fgcolor = _colors[COLOR_SELECT_TEXT];
        } else {
            bgcolor = _colors[COLOR_CLIENT_BKGND];
            fgcolor = _colors[COLOR_CLIENT_TEXT];
        }
        drawBox(g,clientLeft,y,clientWidth,textboxHeihgt,bgcolor,_colors[COLOR_FRAME]);
        setColorOfRGB(g,fgcolor);
        drawText(g,_gooSetPassportPasswd.length() == 0 ? "" : "****",clientLeft,y,clientWidth,textboxHeihgt,space,false,ALIGN_LEFT);
        y += textboxHeihgt + space;

        //  設定ボタン
        if (_gooSetPassportIDSelectedItem == GOO_SET_PASSPORTID_ITEM_OK) {
            bgcolor = _colors[COLOR_ACTIVE_BUTTON_BKGND];
            fgcolor = _colors[COLOR_ACTIVE_BUTTON_TEXT];
        } else {
            bgcolor = _colors[COLOR_BUTTON_BKGND];
            fgcolor = _colors[COLOR_BUTTON_TEXT];
        }
        drawBox(g,clientLeft,y,buttonWidth,buttonHeight,bgcolor,_colors[COLOR_BUTTON_FRAME]);
        setColorOfRGB(g,fgcolor);
        drawText(g,GETMSTR(IDS_GOO_SET_PASSPORTID_OK),clientLeft,y,buttonWidth,buttonHeight,space,false,ALIGN_CENTER);

        //  キャンセルボタン
        if (_gooSetPassportIDSelectedItem == GOO_SET_PASSPORTID_ITEM_CANCEL) {
            bgcolor = _colors[COLOR_ACTIVE_BUTTON_BKGND];
            fgcolor = _colors[COLOR_ACTIVE_BUTTON_TEXT];
        } else {
            bgcolor = _colors[COLOR_BUTTON_BKGND];
            fgcolor = _colors[COLOR_BUTTON_TEXT];
        }
        int cancelButtonLeft = clientLeft + buttonWidth + space + space;
        drawBox(g,cancelButtonLeft,y,buttonWidth,buttonHeight,bgcolor,_colors[COLOR_BUTTON_FRAME]);
        setColorOfRGB(g,fgcolor);
        drawText(g,GETMSTR(IDS_GOO_SET_PASSPORTID_CANCEL),cancelButtonLeft,y,buttonWidth,buttonHeight,1,false,ALIGN_CENTER);
        y += buttonHeight + space;

        //  説明文
        setColorOfRGB(g,_colors[COLOR_TEXT]);
        drawText(g,GETMSTR(IDS_GOO_SET_PASSPORTID_HELP),clientLeft,y,clientWidth,_canvas.getHeight()-y,1,true,ALIGN_LEFT);
    }


    /**
     *  GooPassport設定画面のキー処理
     *  @param key  押下されたキーのコード.
     */
    private static void processSetGooPassportID(int key) {
        if (key == Display.KEY_UP && GOO_SET_PASSPORTID_ITEM_PASSPORTID < _gooSetPassportIDSelectedItem) {
            _gooSetPassportIDSelectedItem--;
            repaint2();
            return;
        }
        if (key == Display.KEY_DOWN && _gooSetPassportIDSelectedItem <= GOO_SET_PASSPORTID_ITEM_OK) {
            _gooSetPassportIDSelectedItem++;
            repaint2();
            return;
        }
        if (key != Display.KEY_SELECT) {
            return;
        }
        switch(_gooSetPassportIDSelectedItem) {
        case GOO_SET_PASSPORTID_ITEM_PASSPORTID:
            String tmp = inputString(_gooSetPassportID,TextBox.ALPHA,TextBox.DISPLAY_ANY,30);
            if (tmp == null) {
                repaint2();
                return;
            }
            _gooSetPassportID = tmp.trim();
            repaint2();
            return;
        case GOO_SET_PASSPORTID_ITEM_PASSWD:
            String tmp2 = inputString("",TextBox.ALPHA,TextBox.DISPLAY_ANY,30);
            if (tmp2 == null) {
                repaint2();
                return;
            }
            _gooSetPassportPasswd = tmp2.trim();
            repaint2();
            return;
        case GOO_SET_PASSPORTID_ITEM_OK:
            if (30 < _gooSetPassportID.length()) {
                showDialog(GETMSTR(IDS_ERROR_GOO_ID),Dialog.DIALOG_INFO);
                return;
            }
            if (30 < _gooSetPassportPasswd.length()) {
                showDialog(GETMSTR(IDS_ERROR_GOO_PASSWD),Dialog.DIALOG_INFO);
                return;
            }
            gooPassportID     = _gooSetPassportID;
            gooPassportPasswd = _gooSetPassportPasswd;
            saveConf();
            protocolArrivalMailCount();
            if (_gooSetPassportID.length() == 0) {
                showDialog(GETMSTR(IDS_CREAR_GOO_ID),Dialog.DIALOG_INFO);
            } else {
                showDialog(GETMSTR(IDS_GOOID_SET_DONE),Dialog.DIALOG_INFO);
            }
            openMain();
            return;
        case GOO_SET_PASSPORTID_ITEM_CANCEL:
            openMain();
            return;
        default:
            ASSERT(false);
            break;
        }
    }
