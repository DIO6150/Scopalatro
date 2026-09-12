#pragma once

class BiTemporalState
{
public:
	bool Is () const { return current; }
	bool Was () const { return previous; }

	void Step () { previous = current; }

	void Listen () { listening = true; cried = false; }
	void Deafen ()
	{
		if (listening && !cried)
		{
			current = false;
		}
		listening = false;
	}

	void Cry ()
	{
		if (listening)
		{
			current = true;
			cried = true;
		}
	}

	void ForceCurrent  (bool value) { current = value; }
	void ForcePrevious (bool value) { previous = value; }

	void ForceAll (bool value)
	{
		current = previous = value;
	}

	void ForceAll (bool valueA, bool valueB)
	{
		current = valueA;
		previous = valueB;
	}
	
private:
	bool current  = false;
	bool previous = false;

	bool listening = false;
	bool cried = false;
};
