-- -------------------------------------
-- Uzura
-- 敵の基本型
-- Actor 構造体に依存
-- 2012 / 09 / 13
-- jskny
-- -------------------------------------


ActorEnemy = {}


function ActorEnemy.new()
	local obj = {
		size = 0.0
	}


	local extend = Actor.new();
	return setmetatable(obj, { __index = extend })
end

