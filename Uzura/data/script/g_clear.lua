-- -------------------------------------
-- Uzura
-- エンドロール出力。
-- 2012 / 11 / 13
-- jskny
-- -------------------------------------


local g_handleBack = 0
local g_timer = 0


-- 背景画面の出力
function DrawBackground()
	-- 描画開始。
--	DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(255, 255, 255), TRUE)
Blend.push()
Blend.set(Blend.ADD, 120)
	DrawBackEn(GetBpm(), 20, GetColor(255, 0, 0), GetColor(255, 50, 0))
Blend.pop()
end


-- シーン開始時に呼ばれる。
function ListenerInit()
	-- アクター初期化。
	InitActors()
	InitEnemys()
	InitEffect()
	InitCamera()
	InitCameraGame()
	InitPlayer()


	g_handleBack = LoadGraph("scene-clear", "endroll.wmv");
	g_timer = GetNowCount()


	-- 動画再生を可能にする。
	PlayMovieToGraph(g_handleBack)
	Sleep(100)
	return true
end


-- シーン終了時に呼ばれる。
function ListenerFinish()
	DeleteGraph(g_handleBack)
	return true
end


-- 画面の出力
function Draw()
	-- 再生停止か？
	if GetMovieStateToGraph(g_handleBack) == 0 or Keybord.isInputNow(Keybord.Z) == true or Joystick.isInputNow(Joystick.A) then
		SendEvent("change-scene", "g_title")
	end


	DrawExtendGraph(0, 0, WINDOW_W, WINDOW_H, g_handleBack, TRUE)


	-- スコア出力
	if (g_timer < (GetNowCount() - 5000)) then
		DrawScore(WINDOW_W - 200, 4)
	end
end


function Update()
end


-- アップデートイベントで呼ばれる。
function ListenerUpdate()
	Update()
	return true
end


-- アップデートイベントで呼ばれる。
function ListenerDraw()
	Draw()
	return true
end


-- 登録処理。
AddListener("scene-init", ListenerInit)
AddListener("scene-finish", ListenerFinish)
AddListener("scene-update", ListenerUpdate)
AddListener("scene-draw", ListenerDraw)


print("Load has Succeed.")
Config("fps", 60) -- fps 50 でいいだろ。


