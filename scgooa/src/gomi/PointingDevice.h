
#if defined(USE_POINTING_DEVICE)
    #ifndef DEBUG
        #define EMU_PROCESS_POINTING_DEVICE
        #define EMU_PAINT_POINTING_DEVICE(g)
        #define POINTING_DEVICE_GET_X()     (PointingDevice.getX())
        #define POINTING_DEVICE_GET_Y()     (PointingDevice.getY())
    #else
        #define EMU_PROCESS_POINTING_DEVICE   do {if(_emuProcessPointingDevice()) { return; } } while(false)
        #define EMU_PAINT_POINTING_DEVICE(g)  do {if(_emuPaintPointingDevice(g))  { return; } } while(false)
        #define POINTING_DEVICE_GET_X()    (_emuPointingDeviceX)
        #define POINTING_DEVICE_GET_Y()    (_emuPointingDeviceY)
    #endif
#else
        #define EMU_PROCESS_POINTING_DEVICE
        #define EMU_PAINT_POINTING_DEVICE(g)
#endif

#if defined(USE_POINTING_DEVICE)
    private static int     _emuPointingDeviceX;
    private static int     _emuPointingDeviceY;
    private static boolean _emuPointingDevice;
    private static Image   _emuPointingDeviceImage;

    /**
     *  DoJaエミュレータ上でタッチパネルの挙動をテストする.
     */
    private static boolean _emuPaintPointingDevice(Graphics g) {
        if (!_emuPointingDevice) {
            return false;
        }
        g.lock();
        g.drawImage(_emuPointingDeviceImage,0,0);

        //  
        String msg = "PointingDevice:" + _emuPointingDeviceX + "," + _emuPointingDeviceY;
        setColorOfRGB(g,0xFFFFFF);
        g.drawString(msg, 0,21);
        g.drawString(msg, 1,20);
        g.drawString(msg,-1,20);
        g.drawString(msg, 0,19);
        setColorOfRGB(g,0x000000);
        g.drawString(msg,0,20);

        //  ポインタ描画
        setColorOfRGB(g,0xFFFFFF);
        g.fillRect(_emuPointingDeviceX-3,_emuPointingDeviceY-3,6,6);
        setColorOfRGB(g,0x000000);
        g.fillRect(_emuPointingDeviceX-2,_emuPointingDeviceY-2,4,4);
        g.unlock(true);
        return true;
    }

    /**
     *  DoJaエミュレータ上でタッチパネルの挙動をテストする.
     */
    private static boolean _emuProcessPointingDevice() {
        int event = keyEvent();
        if (event == Display.KEY_ASTERISK) {
            if (_emuPointingDevice) {
                _emuPointingDeviceImage.dispose();
                _emuPointingDevice = false;
                repaint2();
            } else {
                _emuPointingDeviceImage = Image.createImage(_canvas.getWidth(),_canvas.getHeight());
                Graphics g = _emuPointingDeviceImage.getGraphics();
                _paint(g);
                g.dispose();
                _emuPointingDevice = true;
                repaint2();
            }
            clearKeyEvent();
            return false;
        }

        if (_emuPointingDevice) {
            switch(event) {
            case Display.KEY_LEFT:
                _emuPointingDeviceX--;
                break;
            case Display.KEY_RIGHT:
                _emuPointingDeviceX++;
                break;
            case Display.KEY_UP:
                _emuPointingDeviceY--;
                break;
            case Display.KEY_DOWN:
                _emuPointingDeviceY++;
                break;
            case Display.KEY_SELECT:
            case EVENT_POINTER_SELECTED:
                _keyEvent = EVENT_POINTER_SELECTED;
                return false;
            default:
                return true;
            }
            repaint2();
            return true;
        } else {
            return false;
        }
    }
#endif
