-- -------------------------------------
-- Uzura
-- Actor 基底オブジェクト
-- 2012 / 09 / 13
-- jskny
-- -------------------------------------


Actor = {}


function Actor.serialize(self)
	local str = (self.x .. "," .. self.y .. "," .. self.z)
	return str
end


function Actor.new()
	local obj = {
		pos {
			x = 0, y = 0, z = 0
		},
		type = 0,
		life = 0
	}


	return setmetatable(obj, { __index = Actor })
end


function Actor.update(self)
end


function Actor.draw(self)
end

