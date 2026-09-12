---@diagnostic disable: undefined-global
function OnDamage (event)
	print (event.target.. " got damaged by ".. event.guy.. " for ".. event.base_damage.. " damage")

	return (EventResult.COMPLETE)
end