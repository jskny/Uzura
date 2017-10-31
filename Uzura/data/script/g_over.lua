-- -------------------------------------
-- Uzura
-- おお、勇者よ。死んでしまうとは情けない。
-- 2012 / 11 / 13
-- jskny
-- -------------------------------------


local g_handleSoundBgm = 0
local g_handleBack = 0
local g_timer = 0
local g_handleStart = 0


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
	InitBackEn()


	-- アクター初期化。
	InitActors()
	InitEnemys()
	InitEffect()


	InitCamera()
	InitCameraGame()
	InitPlayer()


	g_handleBack = LoadGraph("scene-over", "back.png");
	g_handleStart = LoadGraph("scene-title", "push_start_button.png");


	g_handleSoundBgm = Sound.load("test0145.ogg")


	g_timer = GetNowCount()
	Config("bpm", 120)
	Sleep(100)
	return true
end


-- シーン終了時に呼ばれる。
function ListenerFinish()
	DeleteGraph(g_handleBack)
	DeleteGraph(g_handleStart)

	Sound.stop(g_handleSoundBgm)
	return true
end


-- 画面の出力
function Draw()
	DrawGraph(0, 0, g_handleBack, TRUE)
Profile.push("background")
	DrawBackground()
Profile.pop()



	-- Pause Any key.
Blend.push()
Blend.set(Blend.ADD, 126)
	local ty = WINDOW_H - 240 + 40 + GetRand(40)
	DrawLine(0, ty, WINDOW_W, ty, GetColor(GetRand(255), GetRand(255), GetRand(255)), 1)
	DrawGraph(WINDOW_W / 2 - 400 / 2, WINDOW_H - 240, g_handleStart, TRUE)
Blend.pop()
--	TaskDraw()
end


local g_flagTmp = 0
function Update()
	if (g_flagTmp == 0) then
		Sound.play(g_handleSoundBgm, Sound.BGM)
		g_flagTmp = 1
	end


	if (GetNowCount() - 60000) > g_timer or Keybord.isInputNow(Keybord.Z) == true or Joystick.isInputNow(Joystick.A) then
		SendEvent("change-scene", "g_title")
	end


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


