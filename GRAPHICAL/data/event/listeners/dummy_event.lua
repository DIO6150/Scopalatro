---@diagnostic disable: undefined-global

PostDeferred (0, {name="mamamia", counter=0})

function OnDummy (event)
	print ("DummyEvent called ".. event.counter)
	event.counter = event.counter + 1

	if event.counter < 10 then
		return (EventResult.CONTINUE)
	end
	
	return (EventResult.COMPLETE)
end

function OnDummy2 (event)
	if event.counter == 5 then
		CancelEvent ()
		print ("I said stop " .. event.counter)
		PostImmediate (1, {target="entity1", guy="entity2", base_damage=10})
		print ("help")
	end

	return (EventResult.COMPLETE)
end