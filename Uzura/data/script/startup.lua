-- -------------------------------------
-- HalCoco
-- 初期化と設定
-- 2012 / 05 / 27
-- jskny
-- -------------------------------------


TRUE = 1
FALSE = 0


print("Uzura startup.")


-- Uzura 設定
Config("debug", 0)
-- プロファイルを出力する。
Config("draw-profile", 0)


-- 音声再生停止？ 1 停止。
Config("sound-off", 0)
-- ウインドウサイズ指定。
Config("WINDOW_W", 800)
Config("WINDOW_H", 600)
-- ウインドウモード 1 full, 0 normal
Config("WINDOW_MODE", 0)


-- キーマップ
-- 0 : jc-u3312sbk, 1 : xbox360
Config("keymap", 0);


-- Vector 演算用のライブラリをロード
DoScript("lib-vector")
DoScript("lib-matrix")
-- Actor を読み込み
DoScript("actor")
DoScript("enemy")


-- 各種スクリプトのリロード
DoScript("reload")


-- title スタート。
SendEvent("change-scene", "g_demo")


print("Uzura startup finished.")

