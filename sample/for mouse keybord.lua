-- -------------------------------------
-- Uzura
-- タイトル画面。
-- 注意
-- LuaPlus はデフォルト引数を処理してはくれないので、
-- Lua から DxLib を呼ぶときは隠しパラメーターも打つ必要がある。
-- 2012 / 09 / 03
-- jskny
-- -------------------------------------


local g_testPos = VGet(0, 0, 0)


-- 背景画面の出力
function DrawBackground()
	DrawLine(0, 0, 500, 500, GetColor(255, 0, 0), 5)
	DrawString(50, 50, "Welcome", GetColor(0, 0, 255), 0)
end


-- アップデートイベントで呼ばれる。
function ListenerUpdateGTitle()
	DrawBackground()


	if Keybord.isOn(Keybord.A) == true then
		print("A")
	end


	if Keybord.isOn(Keybord.B) == true then
		print("B")
	end


	local speed = 5
	if Keybord.isOn(Keybord.LSHIFT) == true then
		speed = 10
	end


	if Keybord.isOn(Keybord.LEFT) == true then
		g_testPos.x = g_testPos.x - speed
	end
	if Keybord.isOn(Keybord.RIGHT) == true then
		g_testPos.x = g_testPos.x + speed
	end
	if Keybord.isOn(Keybord.UP) == true then
		g_testPos.y = g_testPos.y - speed
	end
	if Keybord.isOn(Keybord.DOWN) == true then
		g_testPos.y = g_testPos.y + speed
	end


	DrawCircle(
		g_testPos.x, g_testPos.y, 10, GetColor(255, 0, 0),
		TRUE, 1)


	DrawCircle(
		Mouse.x(), Mouse.y(), 10,
		not Mouse.isOn(Mouse.R) and GetColor(255, 255, 0) or GetColor(0, 255, 0),
		TRUE, 1)


	for i = 1, 10 do
		DrawString(
			100 + Mouse.x(), 300 + 30 * i, "AAA",
			not Mouse.isOn(Mouse.L) and GetColor(11, 50, 255) or GetColor(255, 255, 255)
		, 1)
	end


	return true
end


-- 登録処理。
AddListener("update", ListenerUpdateGTitle)
print("Load has Succeed.")


-- FPS は 30 ぐらいでいいでしょう。 Title ですし。
Config("fps", 60)

