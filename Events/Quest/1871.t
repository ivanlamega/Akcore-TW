CNtlTSTrigger
{
	sm = 1;
	sq = 1;
	rq = 0;
	tid = 1871;
	title = 187102;

	CNtlTSGroup
	{
		gid = 0;

		CDboTSContGCond
		{
			cid = 3;
			prelnk = "2;";
			nolnk = 255;
			rm = 0;
			yeslnk = 100;

			CDboTSCheckSToCEvt
			{
				itype = 0;
			}
		}
		CDboTSContGAct
		{
			cid = 101;
			elnk = 255;
			nextlnk = 254;
			prelnk = "100;";

			CDboTSActSToCEvt
			{
				apptype = 0;
				cnt1 = 0;
				eitype = 0;
				idx0 = -1;
				cnt0 = 0;
				cnt2 = 0;
				ectype = -1;
				etype = 1;
				idx1 = -1;
				taid = 1;
				esctype = 0;
				idx2 = -1;
			}
		}
		CDboTSContProposal
		{
			cancellnk = 253;
			cid = 1;
			cont = 187108;
			gtype = 2;
			oklnk = 2;
			area = 187101;
			goal = 187104;
			sort = 187105;
			prelnk = "0;";
			ds = 1;
			grade = 0;
			rwd = 100;
			title = 187102;
		}
		CDboTSContReward
		{
			canclnk = 255;
			cid = 100;
			desc = 187114;
			nextlnk = 101;
			rwdtbl = 187101;
			rwdtype = 0;
			ltime = -1;
			prelnk = "3;";
			usetbl = 1;

			CDboTSClickNPC
			{
				npcidx = 4071116;
			}
		}
		CDboTSContGAct
		{
			cid = 2;
			elnk = 252;
			nextlnk = 3;
			prelnk = "1;";

			CDboTSActNPCConv
			{
				conv = 187109;
				ctype = 1;
				idx = 4071116;
				taid = 2;
			}
			CDboTSActRegQInfo
			{
				cont = 187108;
				gtype = 2;
				area = 187101;
				goal = 187104;
				m0fz = "69.000000";
				m0widx = 4;
				sort = 187105;
				stype = 1;
				m0fy = "0.000000";
				grade = 132203;
				m0fx = "-132.000000";
				m0ttip = 187115;
				rwd = 100;
				taid = 1;
				title = 187102;
			}
			CDboTSActSToCEvt
			{
				apptype = 0;
				cnt1 = 0;
				eitype = 0;
				idx0 = 1651104;
				cnt0 = 24;
				cnt2 = 0;
				ectype = -1;
				etype = 0;
				idx1 = -1;
				taid = 3;
				esctype = 0;
				idx2 = -1;
			}
		}
		CDboTSContEnd
		{
			cid = 253;
			prelnk = "252;0;1;";
			type = 0;
		}
		CDboTSContEnd
		{
			cid = 254;
			prelnk = "101;";
			type = 1;
		}
		CDboTSContStart
		{
			cid = 0;
			stdiag = 187107;
			nolnk = 253;
			rm = 0;
			yeslnk = 1;

			CDboTSClickNPC
			{
				npcidx = 4071116;
			}
			CDboTSCheckLvl
			{
				maxlvl = 49;
				minlvl = 41;
			}
			CDboTSCheckClrQst
			{
				and = "1870;";
			}
		}
		CDboTSContGAct
		{
			cid = 252;
			elnk = 255;
			nextlnk = 253;
			prelnk = "2;";
		}
	}
}

