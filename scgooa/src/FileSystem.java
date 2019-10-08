/**
 *  $Id: FileSystem.java,v 1.2 2005/03/04 12:14:53 m_yamada Exp $
 *  スクラッチパッド上で
 *      単層ディレクトリ
 *      可変長ファイル
 *  をサポートするファイルシステム.
 * 
 *  ファイルシステムの説明
 *      ディレクトリは単層かつ、
 *      ディレクトリ構造は静的なもので
 *      動的にディレクトリを生成等することはできません。
 *      ディレクトリ内のファイルの情報(名前やサイズ等)を
 *      管理する領域を「ディレクトリエントリ」といいます。
 *      ディレクトリエントリに保存できるファイルの最大数は固定で、
 *      ディレクトリエントリとして、
 *      最大エントリ数 x 各エントリの容量分の領域を予め
 *      スクラッチパッドに準備する必要があります。
 *
 *      ファイルそのものが保存される領域を「データエリア」といいます。
 *      データエリアはディレクトリ間で共有することができます。
 *      データエリアは「ブロック」と呼ばれる単位で使用を管理します。
 *      データエリアのどのブロックが使用済か未使用かを管理する領域を
 *      「ブロックマップ」といいます。
 *      ブロックマップではビットの配列になります。
 *      各ビットが各ブロックに対応し、0なら未使用、1なら使用済みです。
 *      各バイトの低位ビット(0ビット目)から高位ビット(7ビット目)の順で
 *      ブロックの順に対応します。0バイト目の0ビット目はブロック番号0番のブロック
 *      31バイト目の7ビット目はブロック番号255番のブロックに対応します。
 *
 *      ◎  ブロックサイズは4096バイト(4k).
 *      ◎ ブロックマップ領域サイズ
 *          署名             2byte  0x0101  (ver 1.1)
 *          ブロックマップ  32byte  256ブロック / 8ビット
 *      ◎ パーティション毎のエントリ領域サイズ
 *                                1.1/1.0
 *          /sys     8 entry      768/256 byte
 *          /img    16 entry    1,536/512 byte
 *          /snd    16 entry    1,536/512 byte
 *                              3,840 byte
 *      ◎ ファイル毎のディレクトリエントリサイズ.
 *          [ver1.1] (96 byte)
 *               1 byte 削除フラグ
 *               1 byte ブロック長
 *               3 byte ファイル長
 *               8 byte 更新日付
 *              16 byte ファイル名
 *               3 byte 予約領域
 *              64 byte ブロック領域
 *                          各ファイルは、複数の非連続したブロックに分割されます。
 *                          ファイルのデータがどのブロックに格納されているかをこの領域に記録します。
 *                          ブロック番号は1バイトで表されます。
 *                          ファイルのデータが先頭から順に最大64ブロックまで分割されるので、
 *                          それぞれのデータがデータ領域のどのブロックに記録されているか64個分記録されます。
 *                          ブロック領域の0バイト目には、
 *                          ファイルの先頭4Kがどこのブロックに記録されているかを指定するブロック番号。
 *                          ブロック領域の1バイト目には、ファイルの先頭から4kバイト目から4kのデータが・・・。
 *                          という情報が記録されています。
 *          [ver1.0](32 byte)
 *               1 byte 削除フラグ
 *              12 byte ファイル名
 *               1 byte ブロック数
 *               2 byte ファイル長
 *              16 byte ブロック領域
 *      ◎ ファイルの最大サイズ
 *          [ver1.1]
 *          エントリ当りの最大ブロック数は 64ブロック
 *          64 * 4k = 256k
 *          [ver1.0]
 *          16 * 4k = 256k
 *      ◎  パーティション間合計での最大ブロック数
 *          ブロックは最大256個確保可能.
 *      ◎  パーティション間合計の最大容量
 *          データエリアの容量は最大ブロックから1Mバイト.
 *          ブロックの最大からディレクトリ間での最大ファイル数は256個まで.
 */
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import java.util.Calendar;
import java.util.Date;

import com.nttdocomo.ui.*;
/**
 *  スクラッチパッド上で
 *      単層ディレクトリ
 *      可変長ファイル
 *  をサポートするファイルシステムAPI.
 *
 *  @version $Revision: 1.2 $
 *  @author $Author: m_yamada $
 */
public final class FileSystem {
    /**
     *  ファイルシステム署名(バージョン番号).
     */
    private final static int VERSION = 0x0101;
    /**
     *  データブロックサイズ
     */
    final static int BLOCK_BYTES = 4096;
    /**
     *  ディレクトリの各エントリサイズ
     */
    final static int DIRENTRY_BYTES = 96;
    /**
     *  ブロックマップ領域の大きさ
     *  34バイト = 2バイト + MAX_BLOCK(=256) / 8ビット
     */
    final static int BLOCKMAP_BYTES = 32;
    /**
     *  ブロックマップのスクラッチパッド内位置
     */
    private final static int DIRI_BLOCKMAP_POS= 0;
    /**
     *  ディレクトリエントリのスクラッチパッド内位置
     */
    private final static int DIRI_DIRENTRY_POS = 1;
    /**
     *  データ領域のスクラッチパッド内位置
     */
    private final static int DIRI_DATA_POS = 2;
    /**
     *  ディレクトリに含まれるエントリの最大数
     */
    final static int DIRI_MAX_DIRENTRY = 3;
    /**
     *  データ領域に許される最大ブロック数
     */
    private final static int DIRI_MAX_BLOCK = 4;
    /**
     *  ディレクトリエントリ上の削除フラグ
     */
    private final static int DIRENTRY_FLAG_DEL    = 0;
    /**
     *  ディレクトリエントリ上の使用中フラグ
     */
    private final static int DIRENTRY_FLAG_USE = 1;
    /**
     *  ディレクトリエントリ内のフラグフィールドの相対位置
     */
    private final static int DIRENTRY_POS_FLAG       = 0;
    /**
     *  ディレクトリエントリ内のブロック数フィールドの相対位置
     */
    private final static int DIRENTRY_POS_BLOCKCOUNT = 1;
    /**
     *  ディレクトリエントリ内のファイルサイズフィールドの相対位置
     */
    private final static int DIRENTRY_POS_LENGTH     = 2;
    /**
     *  ディレクトリエントリ内の更新日付フィールドの相対位置
     */
    private final static int DIRENTRY_POS_DATE  = 5;
    /**
     *  ディレクトリエントリ内のファイル名領域の相対位置
     */
    private final static int DIRENTRY_POS_NAME       = 13;
    /**
     *  ディレクトリエントリ内のブロック記録領域の相対位置
     */
    private final static int DIRENTRY_POS_BLOCK      = 32;
    /**
     *  最大ファイル長
     */
    private final static int MAX_FILE_LENGTH = 1048576;
    /**
     *  ファイル名の最大長
     */
    final static int MAX_FILE_NAME_LENGTH = 16;
    /**
     *  ファイルあたりの最大ブロック数
     */
    final static int MAX_FILE_BLOCK = 64;

    final static int FILEID_INVALID = 0xFF;

    /**
     *  ディレクトリ構造情報
     */
    private static int _diri[];
    /**
     *  ブロックマップ
     */
    private static byte[] _blockmap;
    /**
     *  ディレクトリエントリ
     */
    private static byte[] _direntry;

    /**
     *  ディレクトリ内のファイルのエントリーID一覧
     */
    private static int[] _fileEntryID;
    /**
     *  ディレクトリ内のファイルのファイル名一覧
     */
    private static String[] _fileName;
    /**
     *  ディレクトリ内のファイルのファイル長一覧
     */
    private static int[] _fileLength;
    /**
     *
     */
    private static Calendar[] _fileDateTime;

    /**
     *  カレントディレクトリを取得する
     */
    static final int[] curDir() {
        return _diri;
    }

    /**
     *  カレントディレクトリを変更する.
     *  @param  diri ディレクトリ構造情報.
     *               指定する情報:
     *                  ブロックマップ位置(ディレクトリ間共通)
     *                  ディレクトリエントリ位置(各ディレクトリ毎)
     *                  データエリア位置(ディレクトリ間共通)
     *                  ブロック換算のデータエリア容量(ディレクトリ間共通)
     *                  ディレクトリの最大エントリ数(各ディレクトリ毎)
     */
    static final int mountDir(int[] diri) {
        if (diri == null) {
            return FILEID_INVALID;
        }
        _diri       = diri;
        _blockmap   = new byte[BLOCKMAP_BYTES];
        _direntry   = new byte[DIRENTRY_BYTES * diri[DIRI_MAX_DIRENTRY]];
        _fileEntryID = null;
        _fileName    = null;
        _fileLength  = null;

        readScratchpad(_diri[DIRI_BLOCKMAP_POS],_blockmap,0,BLOCKMAP_BYTES);
        readScratchpad(_diri[DIRI_DIRENTRY_POS],_direntry,0,_direntry.length);
        return _updateDirEntry();
    }

    /**
     *  ファイルシステムの空き領域を計算する
     *  @return 空き領域のバイト数
     */
    static final int freespace() {
        int freeblocks = 0;
        int maxblocks = _diri[DIRI_MAX_BLOCK];
        int bytes = (maxblocks + 8 - 1)/ 8;
        for (int n = 0;n < bytes;n++) {
            int bits = _blockmap[n];
            for (int b = 0;b < 8;b++) {
                if ((bits & 1) == 0) {
                    freeblocks++;
                }
                bits >>>= 1;
            }
        }
        return freeblocks * BLOCK_BYTES;
    }

    /**
     *  ディレクトリのファイル一覧を取得する.
     *  ファイルの情報(名前、エントリID、サイズ)を静的メンバに格納する.
     *  @see    _fileLength
     *  @see    _fileName
     *  @see    _fileEntryID
     */
    private static final int _updateDirEntry() {
        int maxentry = _diri[DIRI_MAX_DIRENTRY];
        int count = 0;
        for (int n = 0;n < maxentry;n++) {
            int base = DIRENTRY_BYTES * n;
            if (_direntry[base + DIRENTRY_POS_FLAG] == DIRENTRY_FLAG_USE) {
                count++;
            }
        }
        _fileEntryID  = new int[count];
        _fileName     = new String[maxentry];
        _fileLength   = new int[maxentry];
        _fileDateTime = new Calendar[maxentry];
        System.gc();
        int index = 0;
        for (int n = 0;n < maxentry;n++) {
            int base = DIRENTRY_BYTES * n;
            if (_direntry[base + DIRENTRY_POS_FLAG] != DIRENTRY_FLAG_USE) {
                continue;
            }
            _fileEntryID[index] = n;
            _fileName   [n]  = new String(_direntry,base + DIRENTRY_POS_NAME,MAX_FILE_NAME_LENGTH);
            _fileLength [n]  = (int)readUI(_direntry,base + DIRENTRY_POS_LENGTH,3);
            Calendar c = Calendar.getInstance();
            c.setTime(new Date(readUI(_direntry,base + DIRENTRY_POS_DATE,8)));
            _fileDateTime[n] = c;
            index++;
        }
        return count;
    }

    /**
     *  メモリにキャッシュされている
     *      ディレクトリエントリ
     *      ブロックマップ
     *  をスクラッチパッドに書き込む。
     */
    private final static void _sync() {
        writeScratchpad(_diri[DIRI_BLOCKMAP_POS],_blockmap,0,BLOCKMAP_BYTES);
        writeScratchpad(_diri[DIRI_DIRENTRY_POS],_direntry,0,_direntry.length);
        System.gc();
        _updateDirEntry();
    }

    /**
     *  ファイル名を変更する.
     *  @param eid      変更したいファイルを指定するエントリID
     *  @param fname    新しいファイル名
     */
    static final void rename(int eid,String fname) {
        int base = eid * DIRENTRY_BYTES;
        byte[] clear = new byte[MAX_FILE_NAME_LENGTH];
        System.arraycopy(clear,0,_direntry,base + DIRENTRY_POS_NAME,clear.length);
        byte[] sjis = fname.getBytes();
        int length = Math.min(MAX_FILE_NAME_LENGTH,sjis.length);
        System.arraycopy(sjis,0,_direntry,base + DIRENTRY_POS_NAME,length);
        _sync();
    }

    /**
     *  ファイル名を取得する
     *  @param eid ファイル名を取得したいファイルを指定するエントリID
     */
    static final String fileName(int entryid) {
        return _fileName[entryid];
    }

    /**
     *  ディレクトリ内の総ファイル数を返す.
     */
    static final int fileCount() {
        return _fileEntryID.length;
    }

    /**
     *  ディレクトリ内に存在するファイルのエントリ一覧を返す.
     */
    static final int index2Entry(int index) {
        return _fileEntryID[index];
    }

    /**
     *  ファイルのサイズを返す.
     */
    static final int fileLength(int entryid) {
        return _fileLength[entryid];
    }

    /**
     *
     */
    static final Calendar fileDateTime(int entryid) {
        return _fileDateTime[entryid];
    }

    /**
     *  指定したファイルエントリIDが存在するかどうかチェックして
     *  存在する場合はそのファイルエントリID.存在しない場合は
     *  無効子を返す。
     */
    static final int sanitize(int entryid) {
        //  不正
        if (entryid < 0 || _diri[DIRI_MAX_DIRENTRY] <= entryid) {
        }
        int base = DIRENTRY_BYTES * entryid;
        //  既に削除されてやいませんか？
        if (_direntry[base] == DIRENTRY_FLAG_DEL) {
            return FileSystem.FILEID_INVALID;
        }
        return entryid;
    }

    /**
     *  ファイルを作成する
     *  @return FILDEID_INVALID 既にディレクトリエントリ領域は一杯
     *          0- 作成したファイルのエントリID
     */
    static final int createFile() {
        int maxentry = _diri[DIRI_MAX_DIRENTRY];
        for (int eid = 0;eid < maxentry;eid++) {
            int base = DIRENTRY_BYTES * eid;
            if (_direntry[base + DIRENTRY_POS_FLAG] == DIRENTRY_FLAG_DEL) {
                return eid;
            }
        }
        return FileSystem.FILEID_INVALID;
    }

    /**
     *  ファイルにデータを書き込む
     *  @param  eid     エントリID
     *  @param  data    書き込むデータ
     */
    static final boolean writeFile(int eid,byte[] data) {
        if (_diri[DIRI_MAX_DIRENTRY] <= eid) {
            return false;
        }
        if (MAX_FILE_LENGTH <= data.length) {
            return false;
        }

        //  データ領域の更新
        int base = eid * DIRENTRY_BYTES;
        int length  = data.length;
        int nblocks = (length + BLOCK_BYTES - 1) / BLOCK_BYTES;
        int oblocks = (_direntry[base+ DIRENTRY_POS_BLOCKCOUNT] & 0xFF) + 1;
        if (_direntry[base + DIRENTRY_POS_FLAG] == DIRENTRY_FLAG_DEL) {
            oblocks = 0;
        }
        int block = 0;

        //  ファイルサイズの縮小
        if (nblocks < oblocks) {
            for (int n = nblocks;n < oblocks;n++) {
                int blockIndex = _direntry[base + DIRENTRY_POS_BLOCK + n] & 0xFF;
                int bpos = blockIndex / 8;
                int bits = blockIndex % 8;
                _blockmap[bpos] &= ~(1 << bits);
            }
        //  ファイルサイズの拡大
        } else if (oblocks < nblocks) {
            int freeBlocks = 0;
            for(int n = 0;n < _diri[DIRI_MAX_BLOCK];n++) {
                int bpos = n / 8;
                int bits = n % 8;
                if ((_blockmap[bpos] & (1 << bits)) == 0) {
                    freeBlocks++;
                }
            }
            //  空き容量が不足している.
            if (freeBlocks < nblocks - oblocks) {
                return false;
            }
            
            int freeBlockFind = 0;
            for (int n = oblocks;n < nblocks;n++) {
                for(;;) {
                    if (freeBlockFind == _diri[DIRI_MAX_BLOCK]) {
                        return false;
                    }
                    int bpos = freeBlockFind / 8;
                    int bits = freeBlockFind % 8;
                    if ((_blockmap[bpos] & (1 << bits)) == 0) {
                        _blockmap[bpos] |= (1 << bits);
                        _direntry[base + DIRENTRY_POS_BLOCK + n] = (byte)freeBlockFind;
                        break;
                    }
                    freeBlockFind++;
                }
            }
        }

        //  データの更新
        for (int n = 0;n < nblocks;n++) {
            int blockIndex = _direntry[base + DIRENTRY_POS_BLOCK + n] & 0xFF;
            int pos = blockIndex * BLOCK_BYTES;
            int len = Math.min(length - n * BLOCK_BYTES,BLOCK_BYTES);
            writeScratchpad(_diri[DIRI_DATA_POS] + pos,data,n * BLOCK_BYTES,len);
        }
        _direntry[base+DIRENTRY_POS_FLAG]        = DIRENTRY_FLAG_USE;
        writeUI(_direntry,base+DIRENTRY_POS_LENGTH,3,length);
        writeUI(_direntry,base+DIRENTRY_POS_DATE,  8,System.currentTimeMillis());
        _direntry[base+DIRENTRY_POS_BLOCKCOUNT]  = (byte)(nblocks -1);
        _sync();
        return true;
    }

    /**
     *  ファイルを削除する
     *  @param eid エントリID
     */
    static final void deleteFile(int eid) {
        int base = eid * DIRENTRY_BYTES;
        //  既に削除されてやいませんか？
        if (_direntry[base] == DIRENTRY_FLAG_DEL) {
            return;
        }
        //  エントリを無効にし
        _direntry[base] = DIRENTRY_FLAG_DEL;
        //  ブロックマップを解放する
        int blocks  = (_direntry[base+DIRENTRY_POS_BLOCKCOUNT] & 0xFF) + 1;
        for (int n = 0;n < blocks;n++) {
            int blockIndex = _direntry[base + DIRENTRY_POS_BLOCK + n] & 0xFF;
            int bpos = blockIndex / 8;
            int bits = blockIndex % 8;
            _blockmap[bpos] &= ~(1 << bits);
        }
        _sync();
    }

    /**
     *  ファイルを読込む
     *  @param  eid エントリID
     *  @return 読込んだファイルの内容
     *          ファイルが存在しない場合はnullを返す
     *  @param allowEmpty ファイルが存在しない場合はnullではなくbyte[0]を返す
     */
    static final byte[] readFile(int eid,boolean allowEmpty) {
        System.gc();

        int base = eid * DIRENTRY_BYTES;
        if (_direntry[base + DIRENTRY_POS_FLAG] == DIRENTRY_FLAG_DEL) {
            return allowEmpty ? new byte[0] : null;
        }

        int length = (int) readUI(_direntry,base+DIRENTRY_POS_LENGTH,3);
        if (length == 0) {
            return new byte[0];
        }
        int blocks = (_direntry[base+DIRENTRY_POS_BLOCKCOUNT] & 0xFF) + 1;
        byte[] data = new byte[length];
        for (int n = 0;n < blocks;n++) {
            int blockIndex = _direntry[base + DIRENTRY_POS_BLOCK + n] & 0xFF;
            int dataPos    = blockIndex * BLOCK_BYTES;
            int readLen    = n * BLOCK_BYTES;
            int blockLen   = Math.min(length - readLen,BLOCK_BYTES);
            readScratchpad(_diri[DIRI_DATA_POS] + dataPos,data,readLen,blockLen);
        }

        return data;
    }

    /**
     *  スクラッチパッドからデータを読込む
     *
     *  @param scrpos   スクラッチパッド上の位置
     *  @param data     読込んだデータを格納するバイト配列
     *  @param start    バイト配列の格納開始領域の位置
     *  @param length   読込む長さ
     */
    static final void readScratchpad(int scrpos,byte[] data,int start,int length) {
        try {
            InputStream input = Connector.openInputStream("scratchpad:///0;pos=" + scrpos);
            input.read(data,start,length);
            input.close();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    /**
     *  スクラッチパッドにデータを書き込む
     *  @param scrpos   スクラッチパッド上の書込み開始位置
     *  @param data     データ
     *  @param start    データ内の書込み開始位置
     *  @param length   データの長さ
     */
    static final void writeScratchpad(int scrpos,byte[] data,int start,int length) {
        try {
            System.out.println("scrpos=" + scrpos);
            OutputStream output = Connector.openOutputStream("scratchpad:///0;pos=" + scrpos);
            
            output.write(data,start,length);
            output.flush();
            output.close();
        } catch(Exception e) {
            e.printStackTrace();
        }
        
        byte[] read = new byte[length];
        readScratchpad(scrpos,read,0,length);
    }

    /**
     *  ディレクトリの状態をダンプする.
     *  ブロックマップ及び有効なディレクトリエントリを表示する.
     */
    private static final void _dumpDir() {
        //  ブロックマップのダンプ
        System.out.println("[blockmap]");
        StringBuffer blockmap = new StringBuffer();
        for (int n = 0;n < BLOCKMAP_BYTES;n++) {
            String blockmap1 = "00000000" + Integer.toBinaryString(_blockmap[n]);
            blockmap.append(blockmap1.substring(blockmap1.length()-8) + " ");
            if (n % 8 == 7) {
                blockmap.append("\r\n");
            }
        }
        System.out.print(blockmap);

        //  ディレクトリエントリのダンプ
        System.out.println("[direntry]");
        for (int n = 0;n < _diri[DIRI_MAX_DIRENTRY];n++) {
            int base = n * DIRENTRY_BYTES;
            if (_direntry[base + DIRENTRY_POS_FLAG] == DIRENTRY_FLAG_DEL) {
                continue;
            }
            String name = new String(_direntry,base + DIRENTRY_POS_NAME,MAX_FILE_NAME_LENGTH);
            int length = (int)readUI(_direntry,base + DIRENTRY_POS_LENGTH,3);
            int block  = (_direntry[base + DIRENTRY_POS_BLOCKCOUNT] & 0xFF)+ 1;
            StringBuffer line = new StringBuffer();
            line.append("[" + n + "] ");
            line.append(name);
            line.append(" " + length + "バイト (" + block +"ブロック) ");
            for (int b = 0;b < block;b++) {
                line.append("," + Integer.toString(_direntry[base + DIRENTRY_POS_BLOCK + b] & 0xFF));
            }
            System.out.println(line);
        }
    }

    /**
     *  バイト配列に符号無し整数値をビッグエンディアン形式で書き込む
     *  @param data 値を書き込むバイト配列
     *  @param pos バイト配列に書き込む位置
     *  @param b 整数値を表現するバイト数
     *  @param i 書き込む値
     */
    private static final void writeUI(byte[] data,int pos,int b,long i) {
        for(int n = 0;n < b; n++) {
            data[pos + n] = (byte)((i >>> ((b -n -1) * 8)) & 0xff);
        }
        
        if (readUI(data,pos,b) != i) {
            throw new IllegalArgumentException();
        }
    }

    /**
     *  バイト配列から符号無し整数値をビッグエンディアン形式で読込む
     *  @param b    整数値を表現するバイト数
     */
    private static final long readUI(byte[] data,int pos,int b) {
        long i = 0;
        for (int n = 0;n < b;n++) {
            i = (i << 8) + (data[pos + n] & 0xff);
        }
        return i;
    }
    
}