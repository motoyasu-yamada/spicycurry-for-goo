    /**
     *  アイコンフォルダのリスト管理
     */
    private static int[] _iconFolderState = new int[2];

    /**
     *  アイコン設定画面のユーザ操作内容を記録する一時配列
     */
    private static boolean[] tempIconVisible;
    
    private static String[] iconName;

    /**
     *  アイコン一覧画面を開く
     */
    private static void openIconFolder() {
        tempIconVisible = new boolean[iconVisible.length];
        System.arraycopy(iconVisible,0,tempIconVisible,0,iconVisible.length);
        iconName = loadStrings(SRES[IDM_MAIN]);
        initList(_iconFolderState,_font.getHeight() + 4,iconVisible.length,GETMSTR(IDS_CONFIG_ICON));
        _currentScene = SCENE_ICON_FOLDER;
        startTimer();
        activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_CONFIG));
    }

    /**
    *   アイコン一覧を描画する
    */
    private static void paintIconFolderItem(Graphics g,int i,int y,boolean select) {
        int left   = 2 + _font.stringWidth("　") + 2;
        int height = _font.getHeight() + 4;
        int bottom = y + _font.getAscent() + 2;
        final String item = iconName[i];
        if (select) {
            setColorOfRGB(g,_colors[COLOR_SELECT_BKGND]);
            g.fillRect(left,y,_canvas.getWidth() - left,_listLineHeight);
            setColorOfRGB(g,_colors[COLOR_SELECT_TEXT]);
            drawTicker(g,item,getTicker(),left,y,_canvas.getWidth() - left,height,2);
        } else {
            setColorOfRGB(g,_colors[COLOR_TEXT]);
            g.drawString(item,left,bottom);
        }
        if (tempIconVisible[i]) {
            g.setPictoColorEnabled(true);
            g.drawString("\uE70D",2,bottom);
        }
        g.setPictoColorEnabled(false);
    }

    /**
    *   アイコン一覧画面の描画
    */
    private static void paintIconFolder(Graphics g) {
        paintList(g,_currentScene);
    }

    /**
    *   アイコン一覧画面のキー処理
    */
    private static void processIconFolder(int event) {
        if ((event = processList(event)) == EVENT_NULL) {
            return;
        }
        if (event == Display.KEY_UP || event == Display.KEY_DOWN) {
            startTimer();
        }

        int select  = _listState[LIST_STATE_SELECT];
        // 決定キーで表示非表示変更
        if (event == Display.KEY_SELECT) {
            if (select == FileSystem.FILEID_INVALID) {
                showDialog(GETMSTR(IDS_NOT_SELECTED),Dialog.DIALOG_ERROR);
                return;
            }
            setIconVisibility(select);
            _canvas.repaint2();
            return;
        }

        // ソフトキー1で設定を元に戻してTOPへ戻る
        if (event == Display.KEY_SOFT1) {
            openMain();
            return;
        }

        // ソフトキー2で設定を保存して、TOPへ戻る
        if (event == Display.KEY_SOFT2) {
            boolean existsVisibleIcon = 0 < totalVisibleIcon(tempIconVisible);
            if (existsVisibleIcon) {
                System.arraycopy(tempIconVisible,0,iconVisible,0,iconVisible.length);
                saveConf();
                openMain();
            } else {
                showDialog(GETMSTR(IDS_NOICON_SELECTED),Dialog.DIALOG_INFO);
            }
            return;
        }
    }

    /**
     *  アイコンを選択したときの処理
     *  表示だったら非表示に、非表示だったら表示に
     */
    private static void setIconVisibility(int n) {
        int num = totalVisibleIcon(tempIconVisible);
        if (tempIconVisible[n]) {
            tempIconVisible[n] = false;
        } else {
            if (num < ICON_ATV_DISPLAY_NUM) {
                tempIconVisible[n] = true;
            } else {
                showDialog(ICON_ATV_DISPLAY_NUM + "個以上のアイコンを表示することはできません",Dialog.DIALOG_INFO);
            }
        }
    }

    /**
     *  表示されるアイコンの数を計算する。
     */
    private static int totalVisibleIcon(boolean[] icon) {
        int visible = 0;
        for (int n = 0;n < icon.length;n++){
            if (icon[n]) {
                visible++;
            }
        }
        return visible;
    }