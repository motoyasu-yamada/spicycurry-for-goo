/**
 *  $Id: SndFolder.h,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $
 *  音声ディレクトリの画像一覧画面.
 *
 *  キー操作:
 *      1.  元の画面に戻る
 *          左ソフトキー(戻る)  
 *          左カーソル
 *      2.  メニュー
 *          右ソフトキー(ﾒﾆｭ)
 *          右カーソル
 *      3.  閲覧する
 *          決定キー
 *
 *  画面:
 *      画面タイトル
 *      ファイル一覧
 *          アイコン        未使用  なし
 *                          起動    ♪起
 *                          終了    ♪終
 *                          着信    ♪着
 *          ファイル名
 *          ファイルサイズ
 *
 *  メニュー:
 *      起動効果音に設定
 *      終了効果音に設定
 *      着信効果音に設定
 *      音声を削除
 */

    private static int[] _sndFolderState = new int[2];

    /**
     *  音声ディレクトリ画面を開く
     */
    private static void openSndFolder() {
        int count = FileSystem.mountDir(DIRI_SND);
        initList(_sndFolderState,_font.getHeight() + 4,count,GETMSTR(IDS_MAILBOX_SND));
        _currentScene = SCENE_SND_FOLDER;
        activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_SFK_MENU));
        startTimer();
    }

    /**
     *  音声フォルダの一覧の描画
     *  @see paintListItem
     */
    private static void paintSndFolderList(Graphics g,int i,int y,boolean select) {
        int eid = FileSystem.index2Entry(i);
        String stat;
        if (_sndFileIDList[SOUND_INDEX_WELCOME] == eid) {
            stat = "起";
        } else {
            stat = "―";
        }
        if (_sndFileIDList[SOUND_INDEX_BYEBYE] == eid) {
            stat += "終";
        } else {
            stat += "―";
        }
        if (_sndFileIDList[SOUND_INDEX_YOUGOTMAIL] == eid) {
            stat += "着";
        } else {
            stat += "―";
        }
        g.drawString(stat,2,y + _font.getAscent() + 2);

        int left = 2 + _font.stringWidth(stat + " ") + 2;
        String item = FileSystem.fileName(eid);
        if (select) {
            setColorOfRGB(g,_colors[COLOR_SELECT_BKGND]);
            g.fillRect(left,y,_canvas.getWidth() - left,_listLineHeight);
            setColorOfRGB(g,_colors[COLOR_SELECT_TEXT]);
            item += "(サイズ:" + FileSystem.fileLength(eid) + "byte) (更新日:" + format(FileSystem.fileDateTime(eid)) + ")";
            drawTicker(g,item,getTicker(),left,y,_canvas.getWidth() - left,_font.getHeight() + 4,2);
        } else {
            setColorOfRGB(g,_colors[COLOR_TEXT]);
            g.drawString(item,left,y + _font.getAscent() + 2);
        }
    }

    /**
     *  音声ディレクトリ画面のキー処理
     */
    private static void processSndFolder(int event) {
        if (event == Display.KEY_UP || event == Display.KEY_DOWN) {
            startTimer();
        }
        event = processList(event);
        /** m_murai
        if (event == Display.KEY_LEFT ||
            event == Display.KEY_SOFT1) {
            openFolderList();
            return;
        }
        */

        int count = FileSystem.fileCount();
        if (count == 0) {
            return;
        }

        int select = _listState[LIST_STATE_SELECT];
        int entryId = FileSystem.index2Entry(select);
        switch(event) {
        case Display.KEY_SELECT:
            openSndPlayer(entryId,null);
            break;
        case Display.KEY_SOFT2:
            switch(trackMenu(null,loadStrings(MRESOURCES[IDM_SND_DIR]),0,true)) {
            case IDM_SND_DIR_SET:
                trackSetSound(entryId);
                break;
            case IDM_SND_DIR_DEL:
                if (showDialog(GETMSTR(IDS_DEL_CONFIRM),Dialog.DIALOG_YESNO) == Dialog.BUTTON_YES) {
                    FileSystem.deleteFile(entryId);
                    updateList(FileSystem.fileCount());
                    _canvas.repaint2();
                    showDialog(GETMSTR(IDS_DEL_DONE),Dialog.DIALOG_INFO);
                }
            }
            break;
        default:
            break;
        }
        repeatKey();
        _canvas.repaint2();
    }
