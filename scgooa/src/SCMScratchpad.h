    /**
     *  設定情報ファイルのファイルID
     *  旧バージョンの設定情報
     */
    private final static int FILEID_CONF_OLD = 0;
    /**
     *  ブックマークファイルのファイルID
     */
    private final static int FILEID_BOOKMARK = 1;
    /**
     *  設定情報ファイルのファイルID
     */
    private final static int FILEID_CONF     = 2;
    /**
     *  デフォルトで表示するアイコンの数
     */
    private final static int ICON_ATV_DISPLAY_NUM = 6;

    /**
    *   制御情報を読込
    */
    private static void loadConf() {
        iconVisible = new boolean[loadStrings(SRES[IDM_MAIN]).length];
        ByteArrayInputStream input;
        input = readSystemFile(FILEID_CONF_OLD,false);
        if (input != null) {
            TRACE("******** VERSION UP FROM OLD VERSION ************");
            //  自動問合せ間隔
            CURRENT_POLLING_TIME  = readUI16(input);

            //  待受に設定されている画像ファイル一覧を取得する
            int[] saved2 = FileSystem.curDir();
            FileSystem.mountDir(DIRI_IMG);
            _machiukeFileIDList.removeAllElements();
            int num = readUI16(input);
            for (int n = 0;n < num;n++) {
                int id = FileSystem.sanitize(readUI16(input));
                if (id != FileSystem.FILEID_INVALID) {
                    _machiukeFileIDList.addElement(new Integer(id));
                }
            }

            FileSystem.mountDir(saved2);

            _confFontSize = input.read();
            _confDisplayImage = input.read() != 0;
            TOTAL_SAVED_PACKET_FEE =   (input.read() << 24)
                                     + (input.read() << 16)
                                     + (input.read() <<  8)
                                     + (input.read());

            for (int n = 0; n < 9 ; n++) {
                if (input.read() == 1) {
                    iconVisible[n] = true;
                } else {
                    iconVisible[n] = false;
                }
                input.read();
            }
            input.read();
            input.read();

            gooPassportID     = readString16(input);

            FileSystem.mountDir(DIRI_SYS);
            FileSystem.deleteFile(FILEID_CONF_OLD);
            saveConf();
            return;
        }
        
        input = readSystemFile(FILEID_CONF,true);

        //  自動問合せ間隔
        CURRENT_POLLING_TIME  = readUI16(input);

        //  待受に設定されている画像ファイル一覧を取得する
        int[] saved2 = FileSystem.curDir();
        FileSystem.mountDir(DIRI_IMG);
        _machiukeFileIDList.removeAllElements();
        int num = readUI16(input);
        for (int n = 0;n < num;n++) {
            int id = FileSystem.sanitize(readUI16(input));
            if (id != FileSystem.FILEID_INVALID) {
                _machiukeFileIDList.addElement(new Integer(id));
            }
        }

        FileSystem.mountDir(saved2);

        _confFontSize = input.read();
        _confDisplayImage = input.read() != 0;
        TOTAL_SAVED_PACKET_FEE =   (input.read() << 24)
                                 + (input.read() << 16)
                                 + (input.read() <<  8)
                                 + (input.read());

        int visibles = readUI16(input);
        for (int n = 0;n < visibles;n++) {
            int icon = readUI16(input);
            iconVisible[icon] = true;
        }

        gooPassportID     = readString16(input);
        String passwd = readString16(input);
        if (passwd == null) {
            TRACE("versionup");
            return;
        }
        gooPassportPasswd = passwd;

        for (int n = 0;n < privateIconUrl.length;n++) {
            privateIconUrl[n] = readString16(input);
        }

        searchHistory.removeAllElements();
        int historyCount = readUI16(input);
        for (int n = 0;n < historyCount;n++) {
            searchHistory.addElement(readString16(input));
        }
    }

    /**
     *  設定情報を書込
     */
    private static void saveConf() {
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        //  自動接続時間
        writeUI16(output,CURRENT_POLLING_TIME);
        //  待受け画像リストを取得する
        int num = _machiukeFileIDList.size();
        writeUI16(output,num);
        for (int n = 0;n < num;n++) {
            writeUI16(output,((Integer)_machiukeFileIDList.elementAt(n)).intValue());
        }
        //  フォント設定
        output.write(_confFontSize);
        //  ブラウザで画像を表示するかどうか
        output.write(_confDisplayImage ? 1 : 0);
        //  ブラウザでパケット節約額
        output.write((TOTAL_SAVED_PACKET_FEE >> 24) & 0xFF);
        output.write((TOTAL_SAVED_PACKET_FEE >> 16) & 0xFF);
        output.write((TOTAL_SAVED_PACKET_FEE >>  8) & 0xFF);
        output.write( TOTAL_SAVED_PACKET_FEE & 0xFF);

        writeUI16(output,totalVisibleIcon(iconVisible));
        for (int n = 0;n < iconVisible.length;n++) {
            if (iconVisible[n]) {
                writeUI16(output,n);
            }
        }

        writeString16(output,gooPassportID);
        writeString16(output,gooPassportPasswd);

        for (int n = 0;n < privateIconUrl.length;n++) {
            writeString16(output,privateIconUrl[n]);
        }

        int historyCount = searchHistory.size();
        writeUI16(output,historyCount);
        for (int n = 0;n < historyCount;n++) {
            writeString16(output,(String)searchHistory.elementAt(n));
        }

        boolean success = writeSystemFile(FILEID_CONF,output.toByteArray());
        if (!success) {
            showDialog(SystemMessage.FILESYSTEM_OVERSIZE_ERROR,Dialog.DIALOG_ERROR);
        }
    }

    /**
     *  ブックマークを保存する
     */
    private static void saveBookmark() {
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        int count = BOOKMARK_TITLE.size();
        output.write(count);
        for (int n = 0;n < count;n++) {
            writeString16(output,(String)BOOKMARK_TITLE.elementAt(n));
            writeString16(output,(String)BOOKMARK_URL.elementAt(n));
        }
        boolean success = writeSystemFile(FILEID_BOOKMARK,output.toByteArray());
        if (!success) {
            showDialog(SystemMessage.FILESYSTEM_OVERSIZE_ERROR,Dialog.DIALOG_ERROR);
        }
    }

    /**
     *  ブックマークを読込む
     */
    private static void loadBookmark() {
        ByteArrayInputStream input = readSystemFile(FILEID_BOOKMARK,true);

        BOOKMARK_TITLE.removeAllElements();
        BOOKMARK_URL.removeAllElements();

        int count = input.read();
        for (int n = 0;n < count && count != -1;n++) {
            BOOKMARK_TITLE.addElement(readString16(input));
            BOOKMARK_URL.addElement(readString16(input));
        }

        input = null;
        System.gc();
    }

    /**
    *   初回起動時（またはリソース更新時）にアイコンを全てアクティブに設定
    */
    private static void initIconConf() {
        iconVisible    = new boolean[loadStrings(SRES[IDM_MAIN]).length];
        iconVisible[0] = true;
        iconVisible[1] = true;
        iconVisible[6] = true;
        iconVisible[3] = true;
        iconVisible[7] = true;
        iconVisible[4] = true;
    }

    /** 
    *  システムファイルを読込む. 
    *  @param  eid エントリID 
    *  @return 読込んだファイルの内容 
    *          ファイルが存在しない場合はnullを返す 
    *  @param allowEmpty ファイルが存在しない場合はnullではなくbyte[0]を返す 
    */ 
    private static ByteArrayInputStream readSystemFile(int id,boolean allowEmpty) { 
        int[] saved = FileSystem.curDir(); 
        FileSystem.mountDir(DIRI_SYS); 
        byte[] data = FileSystem.readFile(id,allowEmpty);
        FileSystem.mountDir(saved); 
        if (data == null) {
            return null;
        }
        return new ByteArrayInputStream(data); 
    } 
    
    /** 
    *  curDir〜mountDirまでの処理をスレッドセーフに処理する. 
    *  @param id   データの保存先エントリID 
    *  @param data 保存するデータ 
    */ 
    private static boolean writeSystemFile(int fileid,byte[] data) { 
       synchronized(_canvas) { 
           int[] saved = FileSystem.curDir(); 
           FileSystem.mountDir(DIRI_SYS); 
           boolean success = FileSystem.writeFile(fileid,data); 
           FileSystem.mountDir(saved); 
           return success; 
       } 
    }