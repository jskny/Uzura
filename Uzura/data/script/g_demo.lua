-- -------------------------------------
-- Uzura
-- デモ画面 ロゴなり何なり、なんなりと
-- 2012 / 09 / 19
-- jskny
-- -------------------------------------


local g_timer = 0
local g_handleLogo = 0
local g_max = 0
local g_angle = 0


-- シーン開始時に呼ばれる。
function ListenerInit()
	-- N フレーム待つ。
	g_max = 120
	g_timer = g_max


	g_handleLogo = LoadGraph("scene-demo", "logo.png");
	return true
end


-- シーン終了時に呼ばれる。
function ListenerFinish()
	DeleteGraph(g_handleLogo)
	return true
end


-- 画面の出力
function Draw()
	DrawBox(0, 0, WINDOW_W, WINDOW_W, GetColor(255, 255, 255), TRUE)
	DrawGraph(0, 0, g_handleLogo, TRUE)


	local x = 230
	local y = 210
	local color = GetColor(0, 0, 50)
	DrawPolygon2d(x, y, 5, g_angle, 50, color, 4, 1)
	DrawPolygon2d(x, y, 8, g_angle + GetRad(18), 100, color, 3, 1)
	DrawPolygon2d(x, y, 7, g_angle, 140, color, 2, 1)
	DrawPolygon2d(x, y, 7, g_angle, 180, color, 1, 1)
	DrawPolygon2d(x, y, 180, g_angle, 200 + g_timer, GetColor(255, 0, 0), 80, 1, 1)
	DrawPolygon2d(x, y, 14, g_angle, 220, color, 1, 1)

	DrawPolygon2d(230, 210, 6, 0, 100, GetColor(0, 0, 0), 2, 2)
end


-- アップデートイベントで呼ばれる。
local g_flagPlaySound = 0
function ListenerUpdate()
	if Keybord.isOn(Keybord.Z) == true or g_timer <= 20 or Joystick.isInput(Joystick.A) or Joystick.isInput(Joystick.START) then
		-- title へ戻る。
		SendEvent("change-scene", "g_title")
	end


	g_timer = g_timer - 1
	g_angle = g_angle + GetRad(5)
--	DrawLoadingScene()


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


