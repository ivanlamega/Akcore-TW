#include "stdafx.h"
#include "SharedDef.h"
#include "Vector.h"

class CreatureSpawnList
{
public:
	CreatureSpawnList(){};
	~CreatureSpawnList(){};
	CNtlVector			GetPosition();
	void				SetPosition(CNtlVector curPos);
	void				SetID(int _id);
	int					GetID();
	void				SetIsSpawn(bool wat);
	bool				isSpawned();
private:	
	bool				isSpawn;
	CNtlVector			vCurLoc;
	int					ID;
};