    /**
     *  秒のミリ秒数
     */
    private static final int MINUTE_MILLIS = 60000;
#if defined(DEVICE_505)
    private static final int GOO_LOGO_HEIGHT  = 40;
    private static final int SEARCH_AREA_TOP  = 8;
    private static final int MARGIN           = 2;
#else
    private static final int GOO_LOGO_HEIGHT  = 20;
    private static final int SEARCH_AREA_TOP  = 2;
    private static final int MARGIN           = 2;
#endif
    /**
     *  GUI部品番号
     *  検索キーワード
     */
    private static final int MAIN_VCURSOR_SEARCHBOX = 0;
    /**
     *  GUI部品番号
     *  検索履歴矢印
     */
    private static final int MAIN_VCURSOR_SEARCH_HISTORY = 1;
    /**
     *  GUI部品番号
     *  検索ボタン
     */
    private static final int MAIN_VCURSOR_SEARCHBTN = 2;
    /**
     *  GUI部品番号
     *  アイコン領域
     */
    private static final int MAIN_VCURSOR_ICON      = 3;
    /**
     *  GUI部品番号
     *  お知らせ機能
     */
    private static final int MAIN_VCURSOR_INFO      = 4;
    /**
     *  GUI部品番号
     *  トップ画面のニュース
     */
    private static final int MAIN_VCURSOR_NEWS      = 5;
    /**
     *  選択されているトップページ最上部のアイコン
     */
    private static int _mainIconSelect;
    /**
     *  メニューを表示中かどうか
     */
    private static boolean _mainDisplayMenu;
    /**
     *  プライベートアイコンの飛び先
     */
    private static final String[] privateIconUrl = {"","",""};
    /**
     *  アイコンの総数(表示中/非表示のものを含めて)
     */
    private static int _mainIconNum = 9;
    /**
     *  選択されているGUI番号
     */
    private static int _mainVCursor;
    /**
     *  描画開始ニュース位置
     */
    private static int firstDrawNews;
    /**
     *  スクロールせずに描画できるニュース件数
     */
    private static int drawableNews;

    /**
     *  最大検索履歴保存件数
     */
    private static final int MAX_SEARCH_HISTORY = 8;
    /**
     *  検索履歴
     */
    private static final Vector searchHistory = new Vector();
    /**
     *  アイコンファイルの縦のアイコン数
     */
    private static final int ICON_CELL_V = 3;

    /**
     *  メイン画面を開く
     */
    private static void openMain() {
        startTimer();

        _mainIconNum = loadStrings(SRES[IDM_MAIN]).length;
        updateCursor();
        _currentScene   = SCENE_MAIN;
        activateCanvas(GETMSTR(IDS_CONFIG),GETMSTR(IDS_SFK_END));
        if (requireVersionup) {
            execute("sm:versionup");
        }
    }

    /**
     *  カーソルをアップデートする
     */
    private static final void updateCursor() {
        boolean existsIcon = 0 < totalVisibleIcon(iconVisible);
        //  初期カーソル位置を決定する
        if (existsIcon) {
            _mainVCursor = MAIN_VCURSOR_ICON;
            _mainIconSelect = nextVisibleIcon(-1);
        } else {
            _mainVCursor = MAIN_VCURSOR_SEARCHBOX;
        }
        firstDrawNews = 0;
    }

    /**
    *   メイン画面を描画する
    *
    *   @warning paintMainMenu(SCENE_MAIN_MENU)が背景描画の為に
    *            呼び出しています
    *   @see SpicyCanvas.paintMainMenu
    */
    private static void paintMain(Graphics g) {
        ASSERT(_currentScene == SCENE_MAIN);
        int iconSizeL = imgIconL.getHeight() / ICON_CELL_V;
        int iconSizeS = imgIconS.getHeight() / ICON_CELL_V;

        //  背景消去
        drawBox(g,0,0,_canvas.getWidth(),_canvas.getHeight(),_colors[COLOR_MAIN_BKGND],_colors[COLOR_FRAME]);
        //  背景画像描画
        if (imgBack != null) {
            int x = (_canvas.getWidth()  - imgBack.getWidth())  / 2;
            g.drawImage(imgBack,x,0);
        }

        //
        //  検索窓/ボタンを描画
        //
        final int searchBtnWidth  = MARGIN + _font.stringWidth(GETMSTR(IDS_GOO_TOP_SEARCH_BTN)) + MARGIN;
        final int searchBtnHeight = MARGIN + _font.getHeight() + MARGIN;
        final int searchBtnLeft   = _canvas.getWidth() - MARGIN - searchBtnWidth;
        final int searchBtnTop    = SEARCH_AREA_TOP;
        final int searchBoxHeight = MARGIN + _font.getHeight() + MARGIN;
        final int triWidth        = searchBoxHeight;
        final int searchBoxLeft   = _canvas.getWidth() * 33 / 100 ;
        final int searchBoxTop    = SEARCH_AREA_TOP;
        final int searchBoxWidth  = searchBtnLeft - (searchBoxLeft + triWidth + MARGIN);

        //  検索ボタン
        int bgcolor;
        int fgcolor;
        if (_mainVCursor == MAIN_VCURSOR_SEARCHBTN) {
            bgcolor = _colors[COLOR_ACTIVE_BUTTON_BKGND];
            fgcolor = _colors[COLOR_ACTIVE_BUTTON_TEXT];
        } else {
            bgcolor = _colors[COLOR_BUTTON_BKGND];
            fgcolor = _colors[COLOR_BUTTON_TEXT];
        }
        drawBox(g,searchBtnLeft, searchBtnTop, searchBtnWidth, searchBtnHeight,bgcolor,_colors[COLOR_BUTTON_FRAME]);
        setColorOfRGB(g,fgcolor);
        drawText(g,GETMSTR(IDS_GOO_TOP_SEARCH_BTN),searchBtnLeft, searchBtnTop, searchBtnWidth, searchBtnHeight,1,false,ALIGN_CENTER);

        //  検索窓
        if (_mainVCursor == MAIN_VCURSOR_SEARCHBOX) {
            bgcolor = _colors[COLOR_SELECT_BKGND];
            fgcolor = _colors[COLOR_SELECT_TEXT];
        } else {
            bgcolor = _colors[COLOR_CLIENT_BKGND];
            fgcolor = _colors[COLOR_CLIENT_TEXT];
        }
        drawBox(g,searchBoxLeft, searchBoxTop, searchBoxWidth, searchBoxHeight,bgcolor,_colors[COLOR_CLIENT_FRAME]);
        setColorOfRGB(g,fgcolor);
        drawText(g,searchKeyword,searchBoxLeft, searchBoxTop, searchBoxWidth, searchBoxHeight,1,false,ALIGN_LEFT);

        //  検索履歴を描画
        if (_mainVCursor == MAIN_VCURSOR_SEARCH_HISTORY) {
            bgcolor = _colors[COLOR_SELECT_BKGND];
            fgcolor = _colors[COLOR_SELECT_TEXT];
        } else {
            bgcolor = _colors[COLOR_CLIENT_BKGND];
            fgcolor = _colors[COLOR_CLIENT_TEXT];
        }
        final int searchArrowLeft = searchBoxLeft + searchBoxWidth - 1;
        drawBox(g,searchArrowLeft,searchBoxTop,triWidth,searchBoxHeight,bgcolor,_colors[COLOR_CLIENT_FRAME]);
        setColorOfRGB(g,fgcolor);
        fillArrowV(g,
                   searchArrowLeft + triWidth / 2,
                   searchBoxTop + searchBoxHeight - MARGIN - 2,
                   -(triWidth / 2 - 2 - MARGIN));

        //
        //  アイコン・アイコン領域を描画
        //
        //  アイコン領域の背景を描画
        //  アイコン領域の上端のY座標 = 検索窓上端＋検索窓の高さ＋余白
        boolean existsIcon = 0 < totalVisibleIcon(iconVisible);
        int iconAreaTop = Math.max(GOO_LOGO_HEIGHT,searchBoxTop + searchBoxHeight) + MARGIN;
        if (existsIcon) {
            int activeIconLeft = 0;
            int smallIconTop = iconAreaTop + ((iconSizeL - iconSizeS) / 2);
            setColorOfRGB(g,_colors[COLOR_LINE]);
            g.fillRect(0,
                       - (iconSizeL - iconSizeS) / 2 + smallIconTop,
                       _canvas.getWidth(),
                        iconSizeL);
    
            //  大/小アイコンを描画
            int iconDrawX = spaceFromLeft;
            for (int n = 0 ; n <_mainIconNum ; n++) {
                if (!iconVisible[n]) {
                    continue;
                }
                g.drawImage(imgIconS,
                            iconDrawX,
                            smallIconTop,
                            //iconSizeS * n,
                            iconSizeS * (n % 5),
                            //0,
                            iconSizeS * (n / 5),
                            iconSizeS,
                            iconSizeS);
                if (_mainVCursor == MAIN_VCURSOR_ICON && _mainIconSelect == n) {
                    activeIconLeft = iconDrawX;
                }
                iconDrawX    += iconSizeS;
            }
            if (_mainVCursor == MAIN_VCURSOR_ICON) {
                g.drawImage(imgIconL,
                         activeIconLeft - (iconSizeL - iconSizeS) / 2,
                         iconAreaTop,
                         //iconSizeL * _mainIconSelect,
                         iconSizeL * (_mainIconSelect % 5),
                         //0,
                         iconSizeL * (_mainIconSelect / 5),
                         iconSizeL,
                         iconSizeL);
            }
        }

        //
        //  gooメール新着数表示
        //
        int iconAreaBottom  = existsIcon ? iconAreaTop + iconSizeL : iconAreaTop;
        int mailAreaHeight  = MARGIN + _font.getHeight() + MARGIN;
        int gooMailLineY    = iconAreaBottom + mailAreaHeight;
        if (ArrivalMailCount != null) {
            setColorOfRGB(g,_colors[COLOR_TEXT]);
            drawText(g,
                     ArrivalMailCount,
                     spaceFromLeft,
                     gooMailLineY - mailAreaHeight,
                     _canvas.getWidth() - spaceFromLeft,
                     mailAreaHeight,
                     MARGIN,
                     false,ALIGN_LEFT);
        }

        int drawy = gooMailLineY + MARGIN;
        int h = _font.getHeight() + 2;
        //
        //  お知らせを表示する
        //
        if (existsInformation) {
            if (_mainVCursor == MAIN_VCURSOR_INFO) {
                setColorOfRGB(g,_colors[COLOR_SELECT_BKGND]);
            } else {
                setColorOfRGB(g,_colors[COLOR_LINE]);
            }
            g.fillRect(0,
                       drawy,
                       _canvas.getWidth(),
                       h);
            setColorOfRGB(g,_colors[COLOR_SELECT_TEXT]);
            if (_mainVCursor == MAIN_VCURSOR_INFO) {
                drawTicker(g,
                           informationTitle,
                           getTicker(),
                           spaceFromLeft,
                           drawy,
                           _canvas.getWidth()-spaceFromLeft * 2,
                           h,
                           1);
            } else {
                drawText(g,
                         informationTitle,
                         spaceFromLeft,drawy,
                         _canvas.getWidth()-spaceFromLeft*2,
                         h,
                         1,
                         false,
                         ALIGN_LEFT);
            }
            drawy += h + MARGIN;
        } else {
            setColorOfRGB(g,_colors[COLOR_LINE]);
            g.drawLine(0,gooMailLineY,_canvas.getWidth(),gooMailLineY);
        }

        //  ニュース
        int gooNewsTop = drawy;
        //  描画位置がこのラインを超えたら描画しない
#if defined(DEVICE_505)
        // QVGA
        int gooNewsBottom = 212;
#else
        int gooNewsBottom = _canvas.getHeight();
#endif
        //
        //  ニュース一覧を表示する
        //
        setColorOfRGB(g,_colors[COLOR_TEXT]);
        drawableNews = Math.min(NEWS.length,(gooNewsBottom - gooNewsTop) / h);
        //  上にスクロール
        if (0 < firstDrawNews) {
            fillArrowV(g,6,gooNewsTop,4);
        }
        //  下にスクロール
        if (firstDrawNews + drawableNews < NEWS.length) {
            fillArrowV(g,6,gooNewsTop + drawableNews * h,-4);
        }
        for (int n = 0;n < drawableNews;n++) {
            int ni = firstDrawNews + n;
            if (NEWS.length <= ni) {
                break;
            }
            final boolean selected = (_mainVCursor - MAIN_VCURSOR_NEWS) == ni;
            final int y = gooNewsTop + n * h;
            final int w = _canvas.getWidth()-spaceFromLeft*2;
            String  news     = NEWS[ni][0];
            if (selected) {
                setColorOfRGB(g,_colors[COLOR_SELECT_BKGND]);
                g.fillRect(spaceFromLeft,
                           y,
                           w,
                           h);
                setColorOfRGB(g,_colors[COLOR_SELECT_TEXT]);
                drawTicker(g,
                           news,
                           getTicker(),
                           spaceFromLeft,
                           y,
                           w,
                           h,
                           1);
            } else {
                setColorOfRGB(g,_colors[COLOR_TEXT]);
                drawText(g,
                         news,
                         spaceFromLeft,
                         y,
                         w,
                         h,
                         1,
                         false,
                         ALIGN_LEFT);
            }
        }

        if (_mainDisplayMenu) {
            paintMenu(g);
        }
    }

    /**
     *  右のアイコンを取得する.
     */
    private static int nextVisibleIcon(int n) {
        do {
            n = (n + 1) % _mainIconNum;
        } while (!iconVisible[n]);
        return n;
    }

    /**
     *  左のアイコンを取得する.
     */
    private static int prevVisibleIcon(int n) {
        do {
            n = (n + _mainIconNum - 1) % _mainIconNum;
        } while (!iconVisible[n]);
        return n;
    }

    /**
     *  トップページのキーイベント処理
     *  @see processEvent
     */
    private static void processMain(int key) {
        boolean existsIcon = 0 < totalVisibleIcon(iconVisible);
        if (existsIcon && _mainVCursor == MAIN_VCURSOR_ICON) {
            if (key == Display.KEY_LEFT) {
                _mainIconSelect = prevVisibleIcon(_mainIconSelect);
            }
            if (key == Display.KEY_RIGHT) {
                _mainIconSelect = nextVisibleIcon(_mainIconSelect);
            }
        }

        if (_mainDisplayMenu) {
            ASSERT(existsIcon);
            if (key == Display.KEY_RIGHT ||
                key == Display.KEY_LEFT) {
                openMainMenu(_mainIconSelect);
            }

            if (!processMenu(key)) {
                _mainDisplayMenu = false;
                exitMenu();
                startTimer();
                if (0 <= _menuSelect) {
                    menuMain(_menuSelect);
                }
                return;
            }
        } else {
            if (key == Display.KEY_DOWN) {
                startTimer();
                _mainDisplayMenu = false;
                _mainVCursor++;
                if ((!existsIcon) && _mainVCursor == MAIN_VCURSOR_ICON) {
                    //  アイコンが表示されていない場合はアイコンを選択できない
                    _mainVCursor++;
                }
                if ((!existsInformation) && _mainVCursor == MAIN_VCURSOR_INFO) {
                    //  お知らせが表示されていない場合はお知らせを選択できない
                    _mainVCursor++;
                }
                //  画面下端を越えて移動
                if (MAIN_VCURSOR_NEWS + NEWS.length <= _mainVCursor) {
                    _mainVCursor  = 0;
                    firstDrawNews = 0;
                }
                //  ニューススクロール
                if (MAIN_VCURSOR_NEWS <= _mainVCursor &&  
                    MAIN_VCURSOR_NEWS + firstDrawNews + drawableNews <= _mainVCursor) {
                    firstDrawNews++;
                }
                repaint2();
                return;
            }
            if (key == Display.KEY_UP) {
                startTimer();
                _mainDisplayMenu = false;
                _mainVCursor --;
                //  画面上端を越えて移動
                if (_mainVCursor < 0) {
                    _mainVCursor  = MAIN_VCURSOR_NEWS + NEWS.length - 1;
                    firstDrawNews = (_mainVCursor - MAIN_VCURSOR_NEWS) - drawableNews + 1;
                }
                if ((!existsIcon) && _mainVCursor == MAIN_VCURSOR_ICON) {
                    //  アイコンが表示されていない場合はアイコンを選択できない
                    _mainVCursor--;
                }
                if ((!existsInformation) && _mainVCursor == MAIN_VCURSOR_INFO) {
                    //  お知らせが表示されていない場合はお知らせを選択できない
                    _mainVCursor--;
                }
                //  ニューススクロール
                if (MAIN_VCURSOR_NEWS <= _mainVCursor && 
                     _mainVCursor < MAIN_VCURSOR_NEWS + firstDrawNews) {
                    firstDrawNews--;
                }
                repaint2();
                return;
            }
            if (key == Display.KEY_RIGHT ||
                key == Display.KEY_LEFT) {
                repaint2();
                return;
            }
            if (key == Display.KEY_SOFT1) {
                trackConfMenu();
                return;
            }
            if (key == Display.KEY_SOFT2) {
                if (MAPP.getLaunchType() != IApplication.LAUNCHED_AS_CONCIERGE) {
                    MAPP.terminate();
                }
                openMainMachiuke();
                return;
            }
            if (key == Display.KEY_SELECT) {
                switch(_mainVCursor) {
                case MAIN_VCURSOR_SEARCHBTN:
                    if (searchKeyword.length() != 0) {
                        search();
                    }
                    break;
                case MAIN_VCURSOR_SEARCH_HISTORY:
                    searchHistory();
                    break;
                case MAIN_VCURSOR_SEARCHBOX:
                    String input = inputString(searchKeyword, TextBox.KANA, TextBox.DISPLAY_ANY, 255);
                    if (input != null) {
                        searchKeyword = input;
                    }
                    break;
                case MAIN_VCURSOR_ICON:
                    openMainMenu(_mainIconSelect);
                    break;
                case MAIN_VCURSOR_INFO:
                    if (informationURL.length() == 0) {
                        showDialog(informationTitle,Dialog.DIALOG_INFO);
                    } else {
                        execute(informationURL);
                    }
                    break;
                default:
                    execute(NEWS[(_mainVCursor - MAIN_VCURSOR_NEWS)][1]);
                    break;
                }
                return;
            }
        }
    }

    /**
     *  メインメニューを開く
     *  @param どのアイコンのメニューを開くか
     */
    private static void openMainMenu(int open) {
        _mainIconSelect = open;
        _mainDisplayMenu = true;
        String title  = loadStrings(SRES[IDM_MAIN])[_mainIconSelect];
        String[] menu = loadStrings(SRES[IDM_MAIN_ITEM1 + _mainIconSelect]);
        openMenu(title,menu,0,true);
        repaint2();
    }

    /**
     *  設定メニューの処理
     */
    private static void trackConfMenu() {
        String title    = GETMSTR(IDS_MAIN_CONF_MENU_TITLE);
        String[] menu   = loadStrings(SRES[IDM_MAIN_ITEM_CONF]);
        int select = trackMenu(title,menu,0,true);
        if (select == -1) {
            return;
        }

        String cmds = loadStrings(SRES[IDM_MAIN_URL_CONF])[select];
        execute(cmds);
    }


    /**
     *   トップメニュ > 設定
     *   @see    _mainIconSelect
     *   @param  select  選択したメニューアイテム
     *   @return void
     */
    private static void menuMain(int select) {
        String   url = loadStrings(SRES[IDM_MAIN_URL1 + _mainIconSelect])[select];
        execute(url);
    }

    /**
     *  コマンドを実行する.
     *  @param コマンドまたはURL
     */
    private static final void execute(String cmds) {
        //  ニュース自動取得間隔を設定
        if (cmds.equals("sm:confpolling")) {
            int select = -1;
            for (int n = 0;n < POLLIG_TIME_LIST.length;n++) {
                if(CURRENT_POLLING_TIME == POLLIG_TIME_LIST[n]) {
                    select = n;
                }
            }
            select = trackMenu(null,loadStrings(SRES[IDM_POPUP_POLLING_CONF]),select,true);
            if (select != -1) {
                if(showDialog(GETMSTR(IDS_CONFIRM_SET_POLLING),Dialog.DIALOG_YESNO) != Dialog.BUTTON_YES) {
                    return;
                }
                CURRENT_POLLING_TIME = POLLIG_TIME_LIST[select];
                saveConf();
                showDialog(GETMSTR(IDS_CONFIRM_DONE_POLLING),Dialog.DIALOG_INFO);
            }
            return;
        }

        //  ニュースを今更新する
        if (cmds.equals("sm:nowpolling")) {
            protocolGetNews();
            updateCursor();
            repaint2();
            return;
        }

        //  待ち受け画像一覧を開く
        //  m_murai
        if (cmds.equals("sm:imagelist")) {
            openImgFolder();
            return;
        }

        //  表示アイコン設定を開く
        //  m_murai
        if (cmds.equals("sm:iconconfig")) {
            openIconFolder();
            return;
        }

        //  フォントサイズ変更
        if (cmds.equals("sm:fontsize")) {
            int menuSelect = trackMenu(null,loadStrings(SRES[IDM_FONTSIZE]),_confFontSize,true);
            if (menuSelect != -1) {
                changeFont(menuSelect);
                saveConf();
                showDialog(GETMSTR(IDM_SET_FONT_SIZE_DONE),Dialog.DIALOG_INFO);
                repaint2();
            }
            return;
        }

        //  gooPassportID設定画面
        if (cmds.equals("sm:gooidconf")) {
            openGooPassportID(0);
            return;
        }

        if (cmds.equals("sm:sysinfo")) {
            Runtime runtime = Runtime.getRuntime();
            String msg = "FreeMem:"  + runtime.freeMemory()  + "\n" + 
                         "TotalMem:" + runtime.totalMemory() + "\n" + 
                         "FreeFile:" + FileSystem.freespace();
            showDialog(msg,Dialog.DIALOG_INFO);
            return;
        }

        if (cmds.equals("sm:launchappli")) {
            appliLauncher();
            return;
        }

        if (cmds.equals("sm:editprivateicon")) {
            editPrivateIcon();
            return;
        }

        if (cmds.startsWith("sm:privateicon")) {
            int n = Integer.parseInt(cmds.substring("sm:privateicon".length()));
            if (privateIconUrl[n].length() != 0) {
                execute(privateIconUrl[n]);
            }
            return;
        }

        if (cmds.startsWith("sm:versionup")) {
            if (!ignoreVersionup) {
                ignoreVersionup = true;
#if defined(DEVICE_505)
                    MAPP.launch(IApplication.LAUNCH_VERSIONUP,null);
#else
                    showDialog("最新ﾊﾞｰｼﾞｮﾝがあります。ｱﾌﾟﾘ一覧よりﾊﾞｰｼﾞｮﾝｱｯﾌﾟしてください。",Dialog.DIALOG_INFO);
#endif
            }
            return;
        }

        if (cmds.startsWith("sm:tel:")) {
            Phone.call(cmds.substring(7));
            return;
        }

        if (cmds.startsWith("sb:") || cmds.startsWith("http:") || cmds.startsWith("https:")) {
            openBrowser(BROWSER_MODE_STANDARD,cmds);
            return;
        }

    }


    /**
     *  ユーザにアプリ一覧からアプリを選ばせ起動する。
     *  選ばなかった場合は元の画面に戻る。
     */
    private static final void appliLauncher() {
    #if defined(DEVICE_505)
        try {
            ApplicationStore ap = ApplicationStore.selectEntry();
            if (ap == null) {
                return;
            }
            IApplication.getCurrentApp().launch(IApplication.LAUNCH_AS_LAUNCHER,
                                                new String[]{Integer.toString(ap.getId())});
        } catch(Throwable e) {
            e.printStackTrace();
        }
    #else
        showDialog(GETMSTR(IDS_UNSUPPORTED_DOJA2),Dialog.DIALOG_ERROR);
    #endif
    }

    /**
     *  プライベートアイコンを編集する
     */
    private static final void editPrivateIcon() {
        boolean edited = false;
        for (;;) {
            String[] menu = new String[privateIconUrl.length];
            System.arraycopy(loadStrings(SRES[IDM_ICON_MENU_PRIVATE]),0,menu,0,menu.length);
            int n = trackMenu(GETMSTR(IDS_EDIT_PRIVATE_ICON),menu,0,true);
            if (n == -1) {
                break;
            }
            String url = privateIconUrl[n];
            if (url.length() == 0) {
                url = "http://";
            }
            url = inputString(url,TextBox.ALPHA,TextBox.DISPLAY_ANY,255);
            if (url != null && !url.equals("http://")) {
                edited = true;
                privateIconUrl[n] = url;
            }
        }
        if (edited) {
            saveConf();
        }
    }

    /**
     *  プライベートアイコンを登録する
     */
    private static final void setPrivateIcon(String url) {
        int n = trackMenu(GETMSTR(IDS_SET_PRIVATE_ICON),
                          loadStrings(SRES[IDM_ICON_MENU_PRIVATE]),0,true);
        if (n == -1) {
            return;
        }
        if (showDialog(GETMSTR(IDS_SET_PRIVATE_ICON_CONFIRM),Dialog.DIALOG_YESNO) != Dialog.BUTTON_YES) {
            return;
        }
        privateIconUrl[n] = url;
        saveConf();
    }

    /**
     *  検索する
     */
    private static void search() {
        int n = searchHistory.indexOf(searchKeyword);
        if (n != -1) {
            searchHistory.removeElementAt(n);
        }
        searchHistory.insertElementAt(searchKeyword,0);
        while (MAX_SEARCH_HISTORY < searchHistory.size()) {
            searchHistory.removeElementAt(MAX_SEARCH_HISTORY);
        }
        saveConf();
        String url = GETMSTR(IDS_GOO_SEARCH_URL) + URLEncoder.encode(searchKeyword);
        openBrowser(BROWSER_MODE_STANDARD,url);
    }

    /**
     *  検索履歴
     */
    private static void searchHistory() {
        String[] menu = new String[searchHistory.size() + 1];
        searchHistory.copyInto(menu);
        menu[menu.length - 1] = GETMSTR(IDS_SEARCH_HISTORY_CLEAR);
        int n = trackMenu(GETMSTR(IDS_SEARCH_HISTORY),menu,0,true);
        if (n == -1) {
            return;
        }
        if (n == menu.length - 1) {
            searchHistory.removeAllElements();
            return;
        }
        searchKeyword = (String)searchHistory.elementAt(n);
        _mainVCursor = MAIN_VCURSOR_SEARCHBTN;
        repaint2();
    }

    /**
     *  ブラウザのフォームを自動コンプリートする。
     *  @param url オートコンプリート対象のURLの先頭一致
     *  @param values 設定するフォーム
     */
    private static void autocomplete(String url,Hashtable values) {
        if (!url.startsWith(GETMSTR(IDS_AUTOCOMPLETE_DOMAIN))) {
            return;
        }
        if (gooPassportID != null) {
            autoComplete(values,"fm0.fc4.account", gooPassportID);
            autoComplete(values,"fm0.fc3.uname",   gooPassportID);
        }
        if (gooPassportPasswd != null) {
            autoComplete(values,"fm0.fc5.password",gooPassportPasswd);
            autoComplete(values,"fm0.fc4.pass",    gooPassportPasswd);
        }
    }

    /**
     *  フォームの変数値を自動補完する。
     *  変数値が設定されていない場合のみ自動補完する。
     */
    private static void autoComplete(Hashtable values,String key,String value) {
        if (values.containsKey(key) && ((String)values.get(key)).length() != 0) {
            return;
        }
        values.put(key,value);
    }

    /** 
    *   活性化状態から非活性化して待受画面へ遷移
    *   休眠状態から非活性化状態の待受画面へ遷移
    */
    private static void openMainMachiuke() {
        startTimer();
        _currentScene = SCENE_MAIN_MACHIUKE;
        activateCanvas(null,null);
        if(CURRENT_POLLING_TIME != 0) {
            MAPP.setWakeupTimer(CURRENT_POLLING_TIME * MINUTE_MILLIS);
        }
        MAPPLICATION_MODE = MAPPLICATION_MODE_MACHIUKE;
        MAPP.deactivate();
    }

    /**
     *  待受画面の描画
     */
    private static void paintMainMachiuke(Graphics g) {
        ASSERT(Display.getCurrent() == _canvas);
        ASSERT(imgIconL != null);
        ASSERT(_font != null);
        ASSERT(_canvas != null);
        //  メトリクス
        int screenw      = _canvas.getWidth();
        int screenh      = _canvas.getHeight();
        int tickerHeight = _font.getHeight() + 4;
        int tickery      = screenh - tickerHeight;
        //  背景消去
        drawBox(g,0,0,screenw,screenh,_colors[COLOR_MAIN_BKGND],_colors[COLOR_FRAME]);

        //  待受画像の描画
        if (MACHIUKE0 != null) {
            g.drawImage(MACHIUKE0.getImage(),
                        (screenw - MACHIUKE0.getWidth())  / 2,
                        (screenh - MACHIUKE0.getHeight()) / 2);
        }
        // 活性化ボタンを押してね表示
        g.drawImage (imgAlpha,
                     5,
                     tickery - imgAlpha.getHeight()-1);

        drawStringX(g,
                    GETMSTR(IDS_MACHIUKE_KASSEIKA),
                    6 + imgAlpha.getWidth(),
                    tickery  - _font.getDescent() - 1,
                    _colors[COLOR_TEXT],
                    _colors[COLOR_TEXT_FRAME]);
        // ティッカーを表示する
        drawBox(g,0,tickery,screenw,tickerHeight,_colors[COLOR_CLIENT_BKGND],_colors[COLOR_CLIENT_FRAME]);
    
        // ティッカー描画
        setColorOfRGB(g,_colors[COLOR_CLIENT_TEXT]);
        if (NEWS != NEWS_NULL) {
            drawTicker(g,GETMSTR(IDS_LATEST_NEWS) + NEWS[0][0],getTicker(),0,tickery,screenw,tickerHeight,2);
        }
    }
