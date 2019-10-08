
    /**
     *  リスト形式画面の共通ライブラリ
     */

    /**
     *  一覧の選択アイテムインデックス
     */
    private static final int LIST_STATE_SELECT = 0;
    /**
     *  一覧の描画開始アイテムインデックス
     */
    private static final int LIST_STATE_DRAW_START = 1;
    /**
     *  タイトル
     */
    private static String _listTitle;
    /**
     *  一覧のアイテム
     */
    private static int _listItemCount;
    /**
     *  リストの状態
     */
    private static int[]  _listState;
    /**
     *  リストの各行の高さ
     */
    private static int    _listLineHeight;

    /**
     *  リストアイテムを初期化する.
     */
    private static void initList(int[] state,int height,int count,String title) {
        _listTitle      = title;
        _listItemCount  = count;
        _listState      = state;
        _listLineHeight = height;
        updateList(_listItemCount);
    }

    /**
     *  リスト情報を更新する
     */
    private static void updateList(int n) {
        _listItemCount= n;
        int select    = _listState[LIST_STATE_SELECT];
        int start     = _listState[LIST_STATE_DRAW_START];
        //  リスト項目数の減少時に不正な行をポイントしないように調整
        if (_listItemCount == 0) {
            _listState[LIST_STATE_SELECT]      = -1;
            _listState[LIST_STATE_DRAW_START]  = 0;
            return;
        }
        if (select == -1) {
            _listState[LIST_STATE_SELECT]      = 0;
            _listState[LIST_STATE_DRAW_START]  = 0;
            return;
        }
        if (_listItemCount <= select) {
            select = _listItemCount - 1;
        }

        //  スクロール位置の調整
        int titleHeight = _font.getHeight() + 4;
        int visibleLineCount  = (_canvas.getHeight() - titleHeight) / _listLineHeight;
        int botttomLine = start + visibleLineCount -1;
        if (botttomLine < select) {
            start = select - (visibleLineCount -1);
        }
        if (select < start) {
            if (0 <= select) {
                start = select;
            }
        }
        start = (start + _listItemCount) % _listItemCount;
        _listState[LIST_STATE_DRAW_START] = start;
        _listState[LIST_STATE_SELECT]     = select;
    }

    /**
     *  リストアイテムを描画する.
     *  各リストの行の描画をカスタマイズする為のコールバックフック.
     */
    private static void paintListItem(Graphics g,int scene,int i,int y,boolean select) {
        switch(scene) {
        /** m_murai
        case SCENE_FOLDER_LIST:
            paintFolderListItem(g,i,y);
            break;
        case SCENE_MAIL_LIST:
            paintMailListItem(g,i,y,select);
            break;
        case SCENE_ADDRESS_LIST:
            paintAddressListItem(g,i,y);
            break;
        */
        case SCENE_IMG_FOLDER:
            paintImgFolderItem(g,i,y,select);
            break;
        /* m_murai
        case SCENE_SND_FOLDER:
            paintSndFolderList(g,i,y,select);
            break;
        */
        // m_murai 表示アイコン設定
        case SCENE_ICON_FOLDER:
            paintIconFolderItem(g,i,y,select);
            break;
        }
    }

    /**
     *  各種一覧画面を表示する
     *  フォルダ一覧
     *  メール一覧
     *  アドレス一覧
     */
    private static void paintList(Graphics g,int scene) {
        //  背景消去
        drawBox(g,0,0,_canvas.getWidth(),_canvas.getHeight(),_colors[COLOR_BKGND],_colors[COLOR_FRAME]);

        //  タイトルを描画
        paintTitle(g,_listTitle);

        //  アイテムの描画
        if (_listItemCount == 0) {
            return;
        }

        int y = _font.getHeight() + 4 ;
        for (int i = _listState[LIST_STATE_DRAW_START];i < _listItemCount;i++) {
            if(_canvas.getHeight() < y) {
                break;
            }
            setColorOfRGB(g,_colors[COLOR_LINE]);
            g.drawLine(0,y + _listLineHeight,_canvas.getWidth(),y + _listLineHeight);
            boolean select = (i == _listState[LIST_STATE_SELECT]);
            if (select) {
                setColorOfRGB(g,_colors[COLOR_SELECT_BKGND]);
                g.fillRect(0,y,_canvas.getWidth(),_listLineHeight);
                setColorOfRGB(g,_colors[COLOR_SELECT_TEXT]);
            } else {
                setColorOfRGB(g,_colors[COLOR_TEXT]);
            }
            paintListItem(g,scene,i,y,select);
            y += _listLineHeight;
        }
    }

    /**
     *  メニューに共通のキーイベント処理
     *  @param key   押されたキー
     *  @return EVENT_NULL     キーイベントを処理した
     *                         又は処理するイベントがなかった.
     *          EVENT_NULL以外 処理するべきイベント
     */
    private static int processList(int key) {
        if (_listItemCount == 0) {
            return key;
        }

        int titleHeight = _font.getHeight() + 4;
        //  可視行数
        int visibleLineCount  = (_canvas.getHeight() - titleHeight) / _listLineHeight;
        if (key != Display.KEY_DOWN &&
            key != Display.KEY_UP) {
            return key;
        }

        int offset = (key == Display.KEY_UP) ? _listItemCount - 1 : 1;
        _listState[LIST_STATE_SELECT] = (_listState[LIST_STATE_SELECT] + offset) % _listItemCount;
        updateList(_listItemCount);
        repaint2();
        repeatKey();
        return EVENT_NULL;
    }
