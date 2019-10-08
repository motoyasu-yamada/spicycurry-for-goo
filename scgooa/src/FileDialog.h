
    /**
     *  ファイルダイアログで選択/入力したファイル名.
     */
    private static String _fileDialogFileName;

    /**
     *  現在のカレントディレクトリのファイル選択ダイアログを開く
     *  @param save true  保存
     *              false 読込
     *  @return FileSystem.FILEID_INVALID  キャンセルした場合等
     *          0〜 エントリID
     */
    static int fileDialog(boolean save) {
        _fileDialogFileName = null;
        int count  = FileSystem.fileCount();
        int offset = save ? 1 : 0;
        String title;
        String[] menu  = new String[offset + count];
        if (save) {
            title   = GETMSTR(IDS_TITLE_SAVE_FILE);
            menu[0] = SystemMessage.NEW_FILE_SAVE;
        } else {
            title   = GETMSTR(IDS_TITLE_SELECT_FILE);
        }
        for (int n = 0;n < count;n++) {
            int eid = FileSystem.index2Entry(n);
            menu[offset + n] = FileSystem.fileName(eid) +
                               "(サイズ:" + FileSystem.fileLength(eid) + "byte) (更新日:" + format(FileSystem.fileDateTime(eid)) + ")";
        }
        int select = trackMenu(null,menu,0,true);
        if (select == -1) {
            return FileSystem.FILEID_INVALID;
        }
        if (!save) {
            int eid = FileSystem.index2Entry(select);
            _fileDialogFileName = FileSystem.fileName(eid);
            return eid;
        }

        String name;
        int eid;
        if (select == 0) {
            name = SystemMessage.NEW_FILE_NAME;
            eid = FileSystem.createFile();
            System.out.println("+++ CreateFile : " + eid);
            //  新規ファイルを作れない
            if (eid == FileSystem.FILEID_INVALID) {
                showDialog(GETMSTR(IDS_OVER_FILE_COUNT),Dialog.DIALOG_ERROR);
                return FileSystem.FILEID_INVALID;
            }
        } else {
            eid =  FileSystem.index2Entry(select-1);
            name = FileSystem.fileName(eid);
        }
        name = inputString(name,TextBox.KANA,TextBox.DISPLAY_ANY,FileSystem.MAX_FILE_NAME_LENGTH);
        //  ファイル名入力中にキャンセルした
        if (name == null) {
            return FileSystem.FILEID_INVALID;
        }
        _fileDialogFileName = name;
        // FileSystem.rename(eid,name);
        return eid;
    }
