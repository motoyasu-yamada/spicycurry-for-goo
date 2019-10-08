/**
 *  $Id: SndPlayer.h,v 1.1.1.1 2005/03/04 10:31:16 m_yamada Exp $
 *  キー操作:
 *      1.  前の画面へ戻る
 *          左キー
 *          戻るボタン
 *      2.  メニューを開く
 *          右キー
 *
 *  メニュー構成:
 *      添付:
 *          再生
 *          ローカル保存
 *      閲覧：
 *          再生
 *          ローカル保存
 *          起動効果音設定
 *          終了効果音設定
 *          着信効果音設定
 *          削除
 */

    private static int        _sndPlayerFileID;
    /**
     *  音声データ
     */
    private static byte[]     _sndPlayerData;
    /**
     *
     */
    private static int _sndPlayerReturnScene;

    /**
     *  音声プレーヤ画面を開く
     */
    private final static void openSndPlayer(int id,byte[] data) {
        ASSERT(id != FILEID_INVALID && data == null ||
               id == FILEID_INVALID && data != null);

        FileSystem.mountDir(DIRI_SND);

        _sndPlayerFileID = id;
        if (id != FILEID_INVALID) {
            data = FileSystem.readFile(id,false);
        }
        if (data == null) {
            showDialog(GETMSTR(IDS_UNLOADABLE_SND),Dialog.DIALOG_ERROR);
            return;
        }
        MediaSound sound = loadMediaS(data);
        if (sound == null) {
            showDialog(GETMSTR(IDS_UNLOADABLE_SND),Dialog.DIALOG_ERROR);
            return;
        }

        _sndPlayerData = data;
        _sndPlayerReturnScene = _currentScene;
        _currentScene = SCENE_SND_PLAYER;
        activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_SFK_MENU));
        playSound(sound);
    }

    /**
     *  音声プレーヤ画面を描画する
     */
    private final static void paintSndPlayer(Graphics g) {
        //  背景消去
        drawBox(g,0,0,_canvas.getWidth(),_canvas.getHeight(),_colors[COLOR_BKGND],_colors[COLOR_FRAME]);


        //  再生ステータスの描画
        String status = GETMSTR(_mediaStatus == MEDIA_STATUS_PLAYING ? IDS_SND_PLAY : IDS_SND_STOP);
        drawStringX(g,
                    status,
                    (_canvas.getWidth()  - _font.stringWidth(status)) / 2,
                    (_canvas.getHeight() - _font.getHeight()) / 2 + _font.getAscent(),
                    _colors[COLOR_TEXT],
                    _colors[COLOR_TEXT_FRAME]);
    }

    /**
     *  音声プレーヤ画面のキー操作
     */
    private final static void processSndPlayer(int event) {
        switch (event) {
        case Display.KEY_SOFT1:
            stopSound();
            if (_sndPlayerReturnScene == SCENE_MAIL_BODY) {
                // m_murai
                // openMailBody();
            } else {
                openSndFolder();
            }
            return;
        case Display.KEY_SOFT2:
            break;
        default:
            return;
        }

        //  添付音声閲覧
        if (_sndPlayerFileID == FILEID_INVALID) {
            switch(trackMenu(null,loadStrings(MRESOURCES[IDM_SND_PLAYER_ATTACH]),0,true)) {
            case IDM_SND_PLAYER_ATTACH_STOP:
                stopSound();
                break;
            case IDM_SND_PLAYER_ATTACH_START:
                playSound(loadMediaS(_sndPlayerData));
                break;
            case IDM_SND_PLAYER_ATTACH_SAVE:
                saveFile(_sndPlayerData);
                break;
            }
        //  フォルダ一覧から音声閲覧
        } else {
            switch(trackMenu(null,loadStrings(MRESOURCES[IDM_SND_PLAYER_VIEW]),0,true)) {
            case IDM_SND_PLAYER_VIEW_STOP:
                stopSound();
                break;
            case IDM_SND_PLAYER_VIEW_START:
                playSound(loadMediaS(_sndPlayerData));
                break;
            case IDM_SND_PLAYER_VIEW_SET:
                trackSetSound(_sndPlayerFileID);
                break;
            }
        }
    }

    /**
     *  指定した音声ファイルの効果音設定/解除をユーザに選択させる.
     */
    private final static void trackSetSound(int entryid) {
        switch(trackMenu(null,loadStrings(MRESOURCES[IDM_SET_SOUND]),0,true)) {
        case -1:
            return;
        case 0:
            _sndFileIDList[SOUND_INDEX_WELCOME] = entryid;
            showDialog(GETMSTR(IDS_SOUND_SET_WELCOME),Dialog.DIALOG_ERROR);
            break;
        case 1:
            _sndFileIDList[SOUND_INDEX_BYEBYE] = entryid;
            showDialog(GETMSTR(IDS_SOUND_SET_BYEBYE),Dialog.DIALOG_ERROR);
            break;
        case 2:
            _sndFileIDList[SOUND_INDEX_YOUGOTMAIL] = entryid;
            showDialog(GETMSTR(IDS_SOUND_SET_YOUGOTMAIL),Dialog.DIALOG_ERROR);
            break;
        }
        saveConf();
    }