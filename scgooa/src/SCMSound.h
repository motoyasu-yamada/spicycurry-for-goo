    private final static int MEDIA_STATUS_PLAYING  = 1;
    private final static int MEDIA_STATUS_WAIT     = 0;

    /**
     *  現在のメディアの再生状態
     */
    private static int _mediaStatus;
    /**
     *  現在再生中の音声
     */
    private static AudioPresenter _audioPresenter = AudioPresenter.getAudioPresenter();

    /**
     *  音声の再生終了イベントを検地する
     */
    public void mediaAction(MediaPresenter source,int type,int param) {
        synchronized (_audioPresenter) {
            if (type == AudioPresenter.AUDIO_COMPLETE) {
                stopSound();
            }
        }
        synchronized(_sysEvent) {
            _sysEvent.addElement(new Integer(EVENT_SYS_REPAINT));
        }
    }


    /**
     *  再生中の音声を停止する
     */
    private static void stopSound() {
        synchronized(_audioPresenter) {
            if (_mediaStatus == MEDIA_STATUS_WAIT) {
                return;
            }
            _audioPresenter.stop();
            _mediaStatus  = MEDIA_STATUS_WAIT;
            MediaSound sound = (MediaSound)_audioPresenter.getMediaResource();
            unuseMedia(sound);
        }
    }

    /**
     *  指定した音声データを再生する.
     *  @param  再生する音声データ
     */
    private static void playSound(MediaSound data) {
        stopSound();

        _audioPresenter.setSound(data);
        _audioPresenter.setMediaListener(_canvas);
        _mediaStatus = MEDIA_STATUS_PLAYING;

        while(true) {
            try {
                _audioPresenter.play();
                break;
            } catch(Throwable e) {
                sleepThread(100);
            }
        }
    }

    /**
     *  指定した音声ディレクトリに保存されている音声を再生する.
     *  @param  ファイルを指定するディレクトリエントリID
     */
    /* m_murai
    private static void playSoundBySoundID(int soundid) {
        int entryId = _sndFileIDList[soundid];
        if (entryId == FILEID_INVALID) {
            return;
        }
        FileSystem.mountDir(DIRI_SND);
        byte[] data = FileSystem.readFile(entryId,false);
        if (data == null) {
            return;
        }
        MediaSound sound = loadMediaS(data);
        if (sound != null) {
            playSound(sound);
        }
        data = null;
        System.gc();
    }
    */
