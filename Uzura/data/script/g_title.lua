-- -------------------------------------
-- Uzura
-- タイトル画面。
-- 注意
-- LuaPlus はデフォルト引数を処理してはくれないので、
-- Lua から DxLib を呼ぶときは隠しパラメーターも打つ必要がある。
-- 2012 / 09 / 03
-- jskny
-- -------------------------------------


local g_handleBack = 0
local g_handleEffect = 0
local g_handleEffect2 = 0


local g_handleAngou = 0
local g_angouPos = VGet(0, 0, 0)
local g_angouD = VGet(0, 0, 0)
local g_angouCounter = 0

local g_counterEffect = 0
local g_flagCounterAdd = true
local g_posEffect = VGet(0, 0, 0)
local g_posEffect2 = VGet(0, 0, 0)


-- ボタンたち
local g_handleStart = 0


-- 雪のエフェクトの座標
local g_posEffectSnow = {}


-- BGM
local g_handleSoundBgm = 0


-- シーン開始時に呼ばれる。
function ListenerInit()
	g_handleBack = LoadGraph("scene-title", "back.png");
	-- 帯
	g_handleEffect = LoadGraph("scene-title", "effect.png");
	-- ノイズ
	g_handleEffect2 = LoadGraph("scene-title", "effect2.png");


	-- ボタンたち
	g_handleStart = LoadGraph("scene-title", "push_start_button.png");
	g_handleAngou = LoadGraph("scene-title", "bg.png");


	g_posEffect.y = 10


	InitBackEn()
	-- アクター初期化。
	InitActors()
	InitEnemys()
	InitEffect()


	InitCamera()
	InitCameraGame()
	InitPlayer()


	-- 雪のエフェクト準備
	for i = 0, 50 do
		local t = VGet(GetRand(WINDOW_W), GetRand(WINDOW_H), GetColor(200, 200, 200))
		g_posEffectSnow[i] = t
	end


	-- 音声
	g_handleSoundBgm = Sound.load("test0196.ogg")


	-- 初期化よろ～
	g_angouCounter = 1000
end


-- シーン終了時に呼ばれる。
function ListenerFinish()
	DeleteGraph(g_handleBack)
	DeleteGraph(g_handleEffect)
	DeleteGraph(g_handleEffect2)
	DeleteGraph(g_handleStart)
	DeleteGraph(g_handleAngou)


	Sound.stop(g_handleSoundBgm)
--	Sound.delete(g_handleSoundBgm)
end


-- カラフルなノイズを生成
function DrawCrystalNoise()
Blend.push()
Blend.set(Blend.ADD, 200)

	-- 風
	local wind = 0
	if Mouse.x() > WINDOW_W/2 then
		wind = -(1 + Mouse.x() - WINDOW_W/2) / 500
	else
		wind = (1 + WINDOW_W/2 - Mouse.x()) / 500
	end


	wind = wind * 2

	-- 風の強化
	if Mouse.isOn(Mouse.L) then
		wind = wind * 4
	end


	for i = 0, 50 do
		local t = g_posEffectSnow[i];
		DrawCircle(t.x, t.y, 2 + GetScale(6), t.z, TRUE, 1)


		t.x = t.x + wind
		t.y = t.y + GetRand(2) + 5
		if t.y > WINDOW_H then
			t.y = 0
		end


		if t.x > WINDOW_W then
			t.x = t.x - WINDOW_W
		end
		if t.x < 0 then
			t.x = t.x + WINDOW_W
		end


		t.z = GetColor(150, 150, 200 + GetRand(50))
		g_posEffectSnow[i] = t
	end
Blend.pop()
end


-- 背景画面の出力
local g_flagPlaySound = 0
function DrawBackground()
	-- 音声再生開始
	if g_flagPlaySound == 0 then
		Sound.play(g_handleSoundBgm, Sound.BGM)
		g_flagPlaySound = 1
	end


	-- 描画開始。
	DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(255, 255, 255), TRUE)
	DrawGraph(0, 0, g_handleBack, TRUE)
Blend.push()
Blend.set(Blend.ADD, g_counterEffect + 126)
	for i = 0, 3 do
		-- 画像のサイズが小さいので進行方向に少しずらした状態で描画する。
		DrawGraph(g_posEffect.x + 722 * i - 100, g_posEffect.y, g_handleEffect, TRUE)
	end
Blend.pop()


	-- 雲
Blend.push()
Blend.set(Blend.ADD, g_counterEffect)
	for i = 0, 2 do
		DrawGraph(g_posEffect2.x + 800 * i, g_posEffect2.y, g_handleEffect2, TRUE)
	end
Blend.pop()


Blend.push()
Blend.set(Blend.ADD, 120)
	DrawBackEn(GetBpm(), 20, GetColor(255, 0, 0), GetColor(255, 50, 0))
Blend.pop()


	-- Pause Any key.
Blend.push()
Blend.set(Blend.ADD, 126 + g_counterEffect)
	local ty = WINDOW_H - 240 + 40 + GetRand(40)
	DrawLine(0, ty, WINDOW_W, ty, GetColor(GetRand(255), GetRand(255), GetRand(255)), 1)
	DrawGraph(WINDOW_W / 2 - 400 / 2, WINDOW_H - 240, g_handleStart, TRUE)
Blend.pop()


	-- 暗号式出力
Blend.push()
Blend.set(Blend.ADD, 40)
	-- 暗号式出力
	g_angouPos = VAdd(g_angouPos, g_angouD)
	DrawGraph(g_angouPos.x, g_angouPos.y, g_handleAngou, TRUE)
--	DrawRotaGraph(g_angouPos.x, g_angouPos.y, 1.0, g_angouRad, g_handleAngou, TRUE, 0)
Blend.pop()


	-- スコア出力
	DrawScore(WINDOW_W - 200, 4)
end


-- アップデートイベントで呼ばれる。
function ListenerUpdate()
	if Keybord.isInputNow(Keybord.Z) == true or Joystick.isInputNow(Joystick.A) or Joystick.isInputNow(Joystick.START) then
		-- main スタート。
		PlaySE("power14")
--		local a = Sound.load("power14/data.wav")
--		Sound.play(a, Sound.BACK)
		SendEvent("change-scene", "g_main")
	end


	if Keybord.isOn(Keybord.E) == true then
		SendEvent("change-scene", "g_clear")
	end
	if Keybord.isOn(Keybord.T) == true then
		SendEvent("change-scene", "g_twitter")
	end


	local speed = 5
	if Keybord.isOn(Keybord.LSHIFT) == true then
		speed = 10
	end


	-- 背景
	-- 移動処理。
	g_posEffect.x = g_posEffect.x - 5
	if (g_posEffect.x < -722) then
		g_posEffect.x = 0
	end

	g_posEffect2.x = g_posEffect2.x - 3
	if (g_posEffect2.x < -800) then
		g_posEffect2.x = 0
	end


	-- 加算量の増減
	if (g_counterEffect > 255/2) then
		g_flagCounterAdd = 0
		g_counterEffect = 255/2
	end

	if (g_counterEffect < 0) then
		g_counterEffect = 0
		g_flagCounterAdd = 1
	end


	if g_flagCounterAdd == 1 then
		g_counterEffect = g_counterEffect + 1
	else
		g_counterEffect = g_counterEffect - 1
	end


	-- ウインドウよ、暴走しろ。
--[[	SetWindowPosition(
		WINDOW_W / 2 - GetRand(WINDOW_W / 2),
		WINDOW_H / 2 - GetRand(WINDOW_H / 2))
]]


	-- 暗号式処理
	g_angouCounter = g_angouCounter + 1
	if (g_angouCounter >= 550) then
		g_angouCounter = 0
		g_angouD.x = GetRand(2) == 0 and -1-GetRand(1) or 1+GetRand(1)
		g_angouD.y = GetRand(2) == 0 and -1-GetRand(1) or 1+GetRand(1)
		g_angouPos = VGet(-1000-GetRand(1000), -1000-GetRand(1000), 0)
	end


	return true
end


-- 出力
function ListenerDraw()
Profile.push("background")
	DrawBackground()
Profile.pop()


--[[	DrawCircle(
		Mouse.x(), Mouse.y(), 10,
		not Mouse.isOn(Mouse.R) and GetColor(255, 255, 0) or GetColor(0, 255, 0),
		TRUE, 1)]]
	-- ノイズ描画
	DrawCrystalNoise()
end


-- 登録処理。
AddListener("scene-init", ListenerInit)
AddListener("scene-finish", ListenerFinish)
AddListener("scene-update", ListenerUpdate)
AddListener("scene-draw", ListenerDraw)
print("Load has Succeed.")


-- FPS は 30 ぐらいでいいでしょう。 Title ですし。
Config("fps", 60)
Config("bpm", 160)

