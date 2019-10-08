/**
 *  $Id: ImgPlayer.h,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $
 *  キー操作:
 *      1.  前の画面へ戻る
 *          左キー
 *          戻るボタン
 *      2.  メニューを開く
 *          右キー
 *
 *  メニュー構成:
 *      添付:
 *          ローカル保存
 *          ネイティブ保存
 *      閲覧：
 *          ローカル保存
 *          ネイティブ保存
 *          待受設定
 *          削除
 *          メール添付送信
 *          手書き編集
 */

    private static int        _imgPlayerFileID;
    /**
     *
     */
    private static MediaImage _imgPlayer;
    /**
     *  画像の水平方向スクロール
     */
    private static int        _imgPlayerScrollX;
    /**
     *  画像の垂直方向スクロール
     */
    private static int        _imgPlayerScrollY;
    /**
     *  画像データ
     */
    private static byte[]     _imgPlayerData;
    /**
     *  画像プレーヤからの戻り先
     */
    private static int _imgPlayerReturnScene;
    /**
     *
     */
    private static String _imgPlayerTitle;;

    /**
     *  画像プレーヤ画面を開く
     */
    private final static void openImgPlayer(int id,byte[] data) {
        int pos=0;
        ASSERT(id != FileSystem.FILEID_INVALID && data == null ||
               id == FileSystem.FILEID_INVALID && data != null);
        ASSERT(_imgPlayer == null);
        _imgPlayerReturnScene = _currentScene;

        //  ファイルシステムから画像データを読込む
        FileSystem.mountDir(DIRI_IMG);
        _imgPlayerFileID = id;
        if (id != FileSystem.FILEID_INVALID) {

            data = FileSystem.readFile(id,false);
            if (data == null) {
                showDialog(GETMSTR(IDS_UNLOADABLE_IMG),Dialog.DIALOG_ERROR);
                exitImgPlayer();
                return;
            }

#if defined(DEVICE_505)
            MediaImage image = MediaManager.getImage(data);
#else
            FileSystem.writeScratchpad(SCRATCH_TEMPORARY, data, 0, data.length);
            MediaImage image = MediaManager.getImage(SCRATCHPAD_URL+SCRATCH_TEMPORARY);
#endif
        }

        //  画像データをデコードし画像を読込む
#if defined(DEVICE_505)
        _imgPlayer        = loadMediaI(data);
#else 
        _imgPlayer        = loadMediaI(SCRATCH_TEMPORARY);
#endif
        if (_imgPlayer == null) {
            data = null;
            showDialog(GETMSTR(IDS_UNLOADABLE_IMG),Dialog.DIALOG_ERROR);
            exitImgPlayer();
            return;
        }

        _imgPlayerScrollX = (_canvas.getWidth()  - _imgPlayer.getWidth()) / 2;
        _imgPlayerScrollY = (_canvas.getHeight() - (_font.getHeight() + 4) - _imgPlayer.getHeight()) /2;
        _imgPlayerTitle = _imgPlayerFileID == FileSystem.FILEID_INVALID ? GETMSTR(IDS_MAILBOX_IMG)
                                                             : FileSystem.fileName(_imgPlayerFileID);
        _currentScene = SCENE_IMG_PLAYER;
        startTimer();
        activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_SFK_MENU));
    }

    /**
     *  画像プレーヤ画面を描画する
     */
    private final static void paintImgPlayer(Graphics g) {
        //  背景消去
        drawBox(g,0,0,_canvas.getWidth(),_canvas.getHeight(),_colors[COLOR_BKGND],_colors[COLOR_FRAME]);

        //  画像描画
        g.drawImage(_imgPlayer.getImage(),_imgPlayerScrollX,_font.getHeight() + 4 + _imgPlayerScrollY);
        
        paintTitle(g,_imgPlayerTitle);
    }

    /**
     *  画像プレーヤ画面を終了する.
     */
    private final static void exitImgPlayer() {
        unuseMedia(_imgPlayer);
        _imgPlayer     = null;
        _imgPlayerData = null;
        System.gc();
        if (_imgPlayerReturnScene == SCENE_IMG_FOLDER) {
            openImgFolder();
        }
        /** m_murai
        else {
            openMailBody();
        }
        */
    }

    /**
     *  画像プレーヤ画面のキー操作
     */
    private final static void processImgPlayer(int event) {
        switch (event) {
        case Display.KEY_SOFT1:
            exitImgPlayer();
            return;
        case Display.KEY_LEFT:
            _imgPlayerScrollX -= 16;
            _canvas.repaint2();
            return;
        case Display.KEY_UP:
            _imgPlayerScrollY -= 16;
            _canvas.repaint2();
            return;
        case Display.KEY_RIGHT:
            _imgPlayerScrollX += 16;
            _canvas.repaint2();
            return;
        case Display.KEY_DOWN:
            _imgPlayerScrollY += 16;
            _canvas.repaint2();
            return;
        case Display.KEY_SOFT2:
            break;
        default:
            return;
        }

        String[] menu = loadStrings(SRES[IDM_IMG_PLAYER_VIEW]);
        switch(trackMenu(null,menu,0,true)) {
        case IDM_IMG_PLAYER_VIEW_SET_MACHIUKE:

            toggleMachiukeImg(_imgPlayerFileID);
            break;
        }
    }

    /**
     *  待受画像の設定変更
     */
    private final static void toggleMachiukeImg(int id) {
        try {
        Integer i = new Integer(id);
        if (_machiukeFileIDList.contains(i)) {
            if (showDialog(GETMSTR(IDS_MACHIUKE_UNSET_CONFIRM),Dialog.DIALOG_YESNO) == Dialog.BUTTON_YES) {
                _machiukeFileIDList.removeElement(i);
                saveConf();
                loadMachiuke();
                showDialog(GETMSTR(IDS_MACHIUKE_UNSET_DONE),Dialog.DIALOG_INFO);
            }
        } else {
            if (showDialog(GETMSTR(IDS_MACHIUKE_SET_CONFIRM),Dialog.DIALOG_YESNO) == Dialog.BUTTON_YES) {
                _machiukeFileIDList.addElement(i);
                saveConf();
                loadMachiuke();
                showDialog(GETMSTR(IDS_MACHIUKE_SET_DONE),Dialog.DIALOG_INFO);
            }
        }
        } catch(Throwable t) {
            showDialog(_error + "\n" + t.toString(),Dialog.DIALOG_ERROR);
        }
    }

    private static String _error;
