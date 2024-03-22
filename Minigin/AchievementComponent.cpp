#include "AchievementComponent.h"

#include <iostream>

namespace GameEngine
{
	CSteamAchievements::CSteamAchievements(GameObject* pGameObject) :
		BaseComponent(pGameObject),
		m_iAppID(0),
		m_bInitialized(false),
		m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived),
		m_CallbackUserStatsStored(this, &CSteamAchievements::OnUserStatsStored),
		m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
	{
		m_iAppID = SteamUtils()->GetAppID();
		m_Achievement = _ACH_ID(ACH_WIN_ONE_GAME, "Winner");
		m_iNumAchievements = 4;
		RequestStats();
	}

	CSteamAchievements::~CSteamAchievements()
	{

	}

	bool CSteamAchievements::RequestStats()
	{
		// Is Steam loaded? If not we can't get stats.
		if (NULL == SteamUserStats() || NULL == SteamUser())
		{
			return false;
		}
		// Is the user logged on?  If not we can't get stats.
		if (!SteamUser()->BLoggedOn())
		{
			return false;
		}
		// Request user stats.
		return SteamUserStats()->RequestCurrentStats();
	}

	bool CSteamAchievements::SetAchievement(const char* ID)
	{
		// Have we received a call back from Steam yet?
		if (m_bInitialized)
		{
			SteamUserStats()->SetAchievement(ID);
			return SteamUserStats()->StoreStats();
		}
		// If not then we can't set achievements yet
		return false;
	}
	void CSteamAchievements::Notify(const int& messageFromSubject)
	{
		if (messageFromSubject >= m_ScoreAchievement)
		{
			SetAchievement("ACH_WIN_100_GAMES"); 
		}
	
	}
	void CSteamAchievements::OnUserStatsReceived(UserStatsReceived_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			if (k_EResultOK == pCallback->m_eResult)
			{
				std::cout << "Received stats and achievements from Steam\n" << std::endl;
				m_bInitialized = true;

				// load achievements
				for (int iAch = 0; iAch < m_iNumAchievements; ++iAch)
				{
					Achievement_t& ach = m_Achievement;

					SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
					_snprintf_s(ach.m_rgchName, sizeof(ach.m_rgchName), "%s",
						SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
							"name"));
					_snprintf_s(ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s",
						SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
							"desc"));
				}
			}
			else
			{
				char buffer[128];
				_snprintf_s(buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult);
				std::cout << buffer << std::endl;;
			}
		}
	}
	void CSteamAchievements::OnUserStatsStored(UserStatsStored_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			if (k_EResultOK == pCallback->m_eResult)
			{
				std::cout << ("Stored stats for Steam\n") << std::endl;
			}
			else
			{
				char buffer[128];
				_snprintf_s(buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult);
				std::cout << buffer << std::endl;
			}
		}
	}
	void CSteamAchievements::OnAchievementStored(UserAchievementStored_t* pCallback)
	{
		// we may get callbacks for other games' stats arriving, ignore them
		if (m_iAppID == pCallback->m_nGameID)
		{
			std::cout << "Stored Achievement for Steam\n" << std::endl;;
		}
	}
}
