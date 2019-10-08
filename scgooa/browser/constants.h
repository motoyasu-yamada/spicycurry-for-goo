/** 最大にキャッシュされる数 */
#define MAX_CACHE_COUNT   		16
/** 最大履歴数 */
#define MAX_HISTORY_COUNT 		32
/** 登録できるブックマーク数 */
#define MAX_BOOKMARK_COUNT		50
/** width/heightのように絶対ピクセル値又はパーセントを指定できる属性の場合
	PIXEL_OR_PERCENT_0PERCENT〜PIXEL_OR_PERCENT_0PERCENT+100 = 0〜100%
	PIXEL_OR_PERCENT_0PIXEL  〜                              = 0ピクセル〜を表す */
#define PIXEL_OR_PERCENT_0PIXEL   (101)
#define PIXEL_OR_PERCENT_0PERCENT (0)

/** GUI(ラジオボタン等)のイメージサイズ */
#define GUI_IMAGE_SIZE           14
/** GUIイメージ内のチェックボックス画像の位置(左から数えて何番目？)*/
#define GUI_IMAGE_INDEX_CHECKBOX  0
/** GUIイメージ内のラジオ画像の位置(左から数えて何番目？)*/
#define GUI_IMAGE_INDEX_RADIO     4
#define GUI_IMAGE_OFFSET_CHECKED  1
#define GUI_IMAGE_OFFSET_FOCUS    2
/** ドロップダウンリストのボタン */
#define GUI_IMAGE_INDEX_SELECT       8
/** ドロップダウンリストのボタンのフォーカス状態 */
#define GUI_IMAGE_INDEX_SELECT_FOCUS 9

