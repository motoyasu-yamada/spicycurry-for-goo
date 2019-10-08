//  $Id: SCMMenu.h,v 1.2 2005/03/09 02:31:33 m_yamada Exp $

    /**
     *  ソフトキーの状態を保存する.
     */
    private static String[] _softkeys = new String[2];
    /**
     *  メニューアイテム
     */
    private static String[] _menu;
    /**
     *  選択されているメニュー
     */
    private static int _menuSelect;
    /**
     *  メニュータイトル
     */
    private static String _menuTitle;
    /**
     *  スクロール表示をするかどうか
     */
    private static boolean _menuScroll = false;
    /**
     *  描画開始位置
     */
    private static int _menuDrawStart;

    /**
     *  メニューを表示する.
     */
    private static final void paintMenu(Graphics g) {
        final int fontheight  = _font.getHeight();
        final int fontascent  = _font.getAscent();
        final int width       = _canvas.getWidth()   * 4 / 5;
        final int left        = _canvas.getWidth()  / 10;
        final int top         = _canvas.getHeight() / 10;
        final int lineheight  = fontheight + 4;
        final int titleheight = fontheight + 4;
        final int count       = _menu.length;
        final int maxheight   = _canvas.getHeight()  * 4 / 5;
        final int cpp         = Math.min(count,(maxheight - titleheight)/ lineheight);
        final int height      = cpp * lineheight + titleheight;
        final int pages       = (cpp == 0) ? 0 : ((count + cpp - 1) / cpp);
        final int page        = (cpp == 0) ? 0 : (Math.max(0,_menuSelect) / cpp);

        // 背景描画
        drawBox(g,left,top,width,height,_colors[COLOR_CLIENT_BKGND],_colors[COLOR_CLIENT_FRAME]);

        // タイトル描画
        drawBox(g,left,top,width,titleheight,_colors[COLOR_TITLE_BKGND],_colors[COLOR_TITLE_FRAME]);
        setColorOfRGB(g,_colors[COLOR_TITLE_TEXT]);
        int index;
        
        int arrowSize = _font.getHeight() / 2 - 2;
        int arrowx = left + width - 2 - arrowSize;
        int arrowy = top + titleheight / 2;
        //  スクロール方式
        if (_menuScroll) {
            //  下にスクロールできる
            if (_menuDrawStart + cpp < count) {
                fillArrowV(g,arrowx,arrowy + arrowSize,-arrowSize);
            }
            //  上にスクロールできる
            if ( 0 < _menuDrawStart) {
                fillArrowV(g,arrowx,arrowy - arrowSize,arrowSize);
            }
            index = _menuDrawStart;
        //  ページ切替方式(左右矢印を描画)
        } else {
            //  次のページあり
            if (page < pages - 1) {
                fillArrowH(g,arrowx+arrowSize,arrowy,-arrowSize);
            }
            //  前のページあり
            if (0 < page) {
                fillArrowH(g,arrowx-arrowSize,arrowy,arrowSize);
            }
            index = cpp * page;
        }
        drawText(g,_menuTitle,left,top,width - arrowSize - 2,titleheight,2,false,ALIGN_LEFT);

        for (int n = 0;n < cpp && index < count;n ++) {
            int y = top + titleheight + lineheight * n;

            // アクセスキー絵文字
            String item = _menu[index];
            int accessKey = _menuScroll ? index : n;
            if (0 <= accessKey && accessKey < 9) {
                item = String.valueOf((char)(0xE6E2 + accessKey)) + item;
            } else {
                item = '□' + item;
            }
            //  選択背景を表示する
            if (_menuSelect == index) {
                setColorOfRGB(g,_colors[COLOR_SELECT_BKGND]);
                g.fillRect(left,y,width-1,lineheight-1);
                setColorOfRGB(g,_colors[COLOR_SELECT_TEXT]);
                drawTicker(g,item,getTicker(),left,y,width,lineheight,2);
            } else {
                setColorOfRGB(g,_colors[COLOR_CLIENT_TEXT]);
                drawText(g,item,left,y,width,lineheight,2,false,ALIGN_LEFT);
            }
            index ++;
        }
    }

    /**
     *  イベント処理.
     */
    private static final boolean processMenu(int event) {
        final int count       = _menu.length;
        if (event == Display.KEY_SOFT1) {
            _menuSelect = -1;
            return false;
        }
        if (count == 0) {
            return true;
        }
        final int lineheight  = _font.getHeight() + 4;
        final int titleheight = _font.getHeight() + 4;
        final int maxheight   = _canvas.getHeight()  * 4 / 5;
        final int cpp         = Math.min(count,(maxheight - titleheight)/ lineheight);
        final int height      = cpp * lineheight + titleheight;
        //  ページ切替時のみの定数
        final int pages = (count + cpp - 1) / cpp;
        final int page  = _menuSelect / cpp;
        boolean loop = true;
        if (Display.KEY_1 <= event && event <= Display.KEY_9) {
            event = (event - Display.KEY_1) + page * cpp;
            if (event < count) {
                _menuSelect = event;
                loop = false;
            }
        }

        if (event == Display.KEY_SELECT) {
            loop = false;
        }
        if (event == Display.KEY_DOWN) {
            startTimer();
            _menuSelect = (_menuSelect+1) % count;
            updateMenu(cpp);
        }
        if (event == Display.KEY_UP) {
            startTimer();
            _menuSelect = (_menuSelect + count - 1) % count;
            updateMenu(cpp);
        }
        if (!_menuScroll) {
            if (event == Display.KEY_RIGHT) {
                _menuSelect = (page + 1) % pages * cpp;
            }
            if (event == Display.KEY_LEFT) {
                _menuSelect = (page + pages - 1) % pages * cpp;
            }
            _menuDrawStart = page * cpp;
        }
        return loop;
    }

    /**
     *  メニュースクロール位置の更新
     */
    private static void updateMenu(int visibleLine) {
        final int count = _menu.length;
        int botttomLine = _menuDrawStart + visibleLine -1;
        if (botttomLine < _menuSelect) {
            _menuDrawStart = _menuSelect - (visibleLine -1);
        }
        if (_menuSelect < _menuDrawStart) {
            if (0 <= _menuSelect) {
                _menuDrawStart = _menuSelect;
            }
        }
        _menuDrawStart = (_menuDrawStart + count) % count;
    }

    /**
     *  メニューを開く
     */
    private static final void openMenu(String title,Object menuObject,int select,boolean scroll) {
        _menuScroll= scroll;
        _menuTitle = (title == null) ? GETMSTR(IDS_MENU) : title;
        if (menuObject instanceof Vector) {
            int count = ((Vector)menuObject).size();
            _menu = new String[count];
            for (int n = 0;n < count;n++) {
                _menu[n] = (String)((Vector)menuObject).elementAt(n);
            }
        } else {
            _menu = (String[])menuObject;
        }
        _canvas.setSoftLabel(Frame.SOFT_KEY_1,GETMSTR(IDS_SFK_RETURN));
        _canvas.setSoftLabel(Frame.SOFT_KEY_2,null);

        _menuSelect = select;
        _menuDrawStart = 0;
        startTimer();
    }

    /**
     *  メニュー終了処理.
     */
    private static final void exitMenu() {
        _canvas.setSoftLabel(Frame.SOFT_KEY_1,_softkeys[0]);
        _canvas.setSoftLabel(Frame.SOFT_KEY_2,_softkeys[1]);
        _canvas.repaint2();
        clearKeyEvent();
        stopTimer();

    }


    /**
     *  メニュー処理
     *  *** モーダル ***
     *  このメソッドは戻るまでブロッキングされます
     *
     *  @param menuObject メニュー定義配列
     *  @param select 初期選択値
     *  @return ユーザが選択した場合:   メニューのインデックス番号 0〜
     *          キャンセルした場合:     -1
     */
    static final int trackMenu(String title,Object menuObject,int select,boolean scroll) {
        clearKeyEvent();
        Graphics g = _canvas.getGraphics();
        openMenu(title,menuObject,select,scroll);

        boolean loop = true;
        int event = EVENT_NULL;
        for(;;) {
            g.lock();
            g.setFont(_font);
            paintMenu(g);
            g.unlock(true);
            

            if (!loop) {
                break;
            }

            event = keyEvent();
            loop = processMenu(event);
            if (event != EVENT_NULL) {
                repeatKey();
            }
            _tickerCount += 4;
        }
        exitMenu();
        g.dispose();
        g = null;
        System.gc();
        return _menuSelect;
    }

