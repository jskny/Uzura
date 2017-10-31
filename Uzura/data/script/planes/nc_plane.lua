-- -------------------------------------
-- Uzura
-- 飛行機の操作。
-- 2012 / 09 / 18
-- jskny
-- -------------------------------------


function Init()
end


function FInish()
end


function Update()
end


function Draw()
	print("Player draw.")
end


AddListener("player-init", Init)
AddListener("player-finish", Finish)
AddListener("player-update", Update)
AddListener("player-draw", Draw)

