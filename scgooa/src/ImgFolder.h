    private static int[] _imgFolderState = new int[2];

    /**
     *  画像ディレクトリ画面を開く
     */
    private static void openImgFolder() {
        int count = FileSystem.mountDir(DIRI_IMG);
        initList(_imgFolderState,_font.getHeight() + 4,count,GETMSTR(IDS_MAILBOX_IMG));
        _currentScene = SCENE_IMG_FOLDER;
        startTimer();
        activateCanvas(GETMSTR(IDS_SFK_RETURN),GETMSTR(IDS_SFK_MENU));
    }

    /**
     *  画像フォルダの各一覧を描画する
     */
    private static void paintImgFolderItem(Graphics g,int i,int y,boolean select) {
        FileSystem.mountDir(DIRI_IMG);
        int eid = FileSystem.index2Entry(i);
        if (_machiukeFileIDList.contains(new Integer(eid))) {
            g.setPictoColorEnabled(true);
            g.drawString("\uE70D",2,y + _font.getAscent() + 2);
            g.setPictoColorEnabled(false);
        }

        int left = 2 + _font.stringWidth("　") + 2;
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
     *  画像ディレクトリ画面の描画
     */
    private static void paintImgFolder(Graphics g) {
        paintList(g,_currentScene);
    }

    /**
     *  画像ディレクトリ画面のキー処理
     */
    private static void processImgFolder(int event) {
        if (event == Display.KEY_UP || event == Display.KEY_DOWN) {
            startTimer();
        }
        if ((event = processList(event)) == EVENT_NULL) {
            return;
        }
        // ソフトキー1でTOPへ
        if (event == Display.KEY_SOFT1) {
            openMain();
            return;
        }

        int select  = _listState[LIST_STATE_SELECT];
        int entryId = (select == -1) ? FileSystem.FILEID_INVALID : FileSystem.index2Entry(select);
        if (event == Display.KEY_SELECT) {
            if (entryId != FileSystem.FILEID_INVALID) {
                openImgPlayer(entryId,null);
            }
            return;
        }
        if (event == Display.KEY_SOFT2) {
            String[] menu  = loadStrings(SRES[IDM_IMG_DIR]);
            switch(trackMenu(null,menu,0,true)) {
            case IDM_IMG_DIR_COPY:
                if (entryId == FileSystem.FILEID_INVALID) {
                    showDialog(GETMSTR(IDS_NOT_SELECTED),Dialog.DIALOG_ERROR);
                    return;
                }
                saveFile(FileSystem.readFile(entryId,false));
                updateList(FileSystem.fileCount());
                _canvas.repaint2();
                return;
#if defined(USE_IMAGE_STORE)
            case IDM_IMG_DIR_NATIVE_IMPORT:
                byte[] imported = selectFromImageStore();
                FileSystem.mountDir(DIRI_IMG);
                saveFile(imported);
                updateList(FileSystem.fileCount());
                _canvas.repaint2();
                return;
            case IDM_IMG_DIR_NATIVE_EXPORT:
                if (entryId == FileSystem.FILEID_INVALID) {
                    showDialog(GETMSTR(IDS_NOT_SELECTED),Dialog.DIALOG_ERROR);
                    return;
                }
                try {
                    byte[] data = FileSystem.readFile(entryId,false);
                    MediaImage image = MediaManager.getImage(data);
                    image.use();
                    ImageStore.addEntry(image);
                    image.unuse();
                    image.dispose();
                    showDialog(GETMSTR(IDS_SAVE_SUCCESS),Dialog.DIALOG_INFO);
                } catch (Exception e) {
                    showDialog(GETMSTR(IDS_SAVE_ERROR),Dialog.DIALOG_ERROR);
                }
                return;
#endif
#if defined(USE_CAMERA)
            case IDM_IMG_DIR_CAMERA_IMPORT:
                FileSystem.mountDir(DIRI_IMG);
                saveFile(takeFromCamera());
                updateList(FileSystem.fileCount());
                _canvas.repaint2();
                return;
#endif
            case IDM_IMG_DIR_DEL:
                if (entryId == FileSystem.FILEID_INVALID) {
                    showDialog(GETMSTR(IDS_NOT_SELECTED),Dialog.DIALOG_ERROR);
                    return;
                }

                if (showDialog(GETMSTR(IDS_DEL_CONFIRM),Dialog.DIALOG_YESNO) != Dialog.BUTTON_YES) {
                    return;
                }
                if (_machiukeFileIDList.removeElement(new Integer(entryId))) {
                    showDialog(GETMSTR(IDS_MACHIUKE_UNSET_DONE),Dialog.DIALOG_INFO);
                }
                FileSystem.deleteFile(entryId);
                updateList(FileSystem.fileCount());
                _canvas.repaint2();
                showDialog(GETMSTR(IDS_DEL_DONE),Dialog.DIALOG_INFO);
                return;
            case IDM_IMG_DIR_SET_MACHIUKE:
                if (entryId == FileSystem.FILEID_INVALID) {
                    showDialog(GETMSTR(IDS_NOT_SELECTED),Dialog.DIALOG_ERROR);
                    return;
                }
                toggleMachiukeImg(entryId);
                _canvas.repaint2();
                return;
            }
            return;
        }
    }

#if defined(USE_CAMERA)
    /**
     *  カメラで画像を撮影し画像をバイト配列形式で取得する.
     *  @return キャンセル/エラーが発生した場合はnull
     */
    private final static byte[] takeFromCamera() {
        Camera camera = Camera.getCamera(0);

        //  液晶サイズ以下の撮影サイズのみピックアップし
        //  その中からユーザに撮影サイズを選択させる.
        int[][] sizes = camera.getAvailablePictureSizes();
        Vector menu = new Vector();
        for (int n = 0;n < sizes.length;n++) {
            if (sizes[n][0] <= _canvas.getWidth() &&
                sizes[n][1] <= _canvas.getHeight()) {
                menu.addElement(sizes[n][0] + "x" + sizes[n][0]);
            }
            
        }
        int select = trackMenu(null,menu,0,true);
        if (select == -1) {
            return null;
        }
        camera.setImageSize(sizes[select][0],sizes[select][1]);

        //  撮影し画像データを取得する
        try {
            camera.takePicture();
            if (camera.getNumberOfImages() == 0) {
                return null;
            }
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            InputStream input = camera.getInputStream(0);
            int b;
            while ((b = input.read()) != -1) {
                output.write(b);
            }
            input.close();
            camera.disposeImages();
            return output.toByteArray();
        } catch(Exception e) {
            camera.disposeImages();
            showDialog(GETMSTR(IDS_CAMERA_TAKE_PICTURE_ERROR),Dialog.DIALOG_ERROR);
            return null;
        }
    }
#endif

    /**
     *  ファイルを保存する.
     */
    private final static int saveFile(byte[] data) {
        if (data == null) {
            showDialog(GETMSTR(IDS_SAVE_ERROR),Dialog.DIALOG_ERROR);
            return FileSystem.FILEID_INVALID;
        }
        int save = fileDialog(true);
        if (save == FileSystem.FILEID_INVALID) {
            return FileSystem.FILEID_INVALID;
        }

        if (!FileSystem.writeFile(save,data)) {
            showDialog(GETMSTR(IDS_SAVE_ERROR),Dialog.DIALOG_ERROR);
            return FileSystem.FILEID_INVALID;
        }

        FileSystem.rename(save,_fileDialogFileName);
        showDialog(GETMSTR(IDS_SAVE_SUCCESS),Dialog.DIALOG_INFO);
        return save;
    }

#if defined(USE_IMAGE_STORE)
    /**
     *  画像を携帯の画像フォルダから取得する
     *  @return キャンセル又はエラーが発生して取得できない場合は
     *          nullを返す.
     */
    private final static byte[] selectFromImageStore() {
        try {
            ImageStore store = ImageStore.selectEntry();
            if (store == null) {
                return null;
            }
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            InputStream input = store.getInputStream();
            int b;
            while ((b = input.read()) != -1) {
                output.write(b);
            }
            input.close();
            byte[] data = output.toByteArray();
            return data;
        } catch(Exception e) {
            e.printStackTrace();
            showDialog(GETMSTR(IDS_IMAGE_STORE_SELECT_ERROR),Dialog.DIALOG_ERROR);
            return null;
        }
    }
#endif
