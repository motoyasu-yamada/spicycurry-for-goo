//*********************************************************
//  スクラッチパッド設計
//  -------------------------------------------------------
//  位置    容量    内容
//  -------------------------------------------------------
//  0       6K      文字列リソース
//  6144    10K     画像リソース
//  20480   1024    ブロックマップ
//  21504   1024    システムディレクトリ
//  22528   5120    イメージディレクトリ
//  34816   56K     データ領域(14ブロック)
//  92160   10K     文字列リソース
//*********************************************************
private static final int SPPOS_SRES        = 0;
private static final int SPPOS_IRES        = 1024 * 6;
private static final int SCRATCH_TEMPORARY = 92160;
private static final int[] DIRI_SYS = new int[]{20480,21504,34816,8,14};
private static final int[] DIRI_IMG = new int[]{20480,22528,34816,8,14};
#define MAX_SCRATCHPAD_LENGTH   102400
// #define UNUSE_CAMERA
// #define UNUSE_IMAGE_STORE
// #define UNUSE_APPLI_LAUNCHER
// #define DEVICE_505
