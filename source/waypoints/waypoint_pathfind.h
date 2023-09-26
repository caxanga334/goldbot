#ifndef WAYPOINT_BASE_PATH_FINDING_H_
#define WAYPOINT_BASE_PATH_FINDING_H_

#include <queue>
#include <vector>
#include <unordered_map>
#include "waypoint_base.h"
#include "waypoint_manager.h"

class CAStarNode
{
public:
	CAStarNode()
	{
		m_my = nullptr;
		m_parent = nullptr;
		m_h = 0.0f;
		m_g = 0.0f;
		m_closed = false;
		m_open = false;
	}

	CAStarNode(CWaypoint* my)
	{
		m_my = my;
		m_parent = nullptr;
		m_h = 0.0f;
		m_g = 0.0f;
		m_closed = false;
		m_open = false;
	}

	bool operator==(const CAStarNode& other) const { return this->m_my == other.m_my; }
	bool operator!=(const CAStarNode& other) const { return this->m_my != other.m_my; }
	void SetParent(CAStarNode* value) { m_parent = value; }
	CWaypoint* GetMyWaypoint() const { return m_my; }
	CAStarNode* GetParent() const { return m_parent; }
	void SetH(float v) { m_h = v; }
	void SetG(float v) { m_g = v; }
	float GetH() const { return m_h; }
	float GetG() const { return m_g; }
	float GetF() const { return m_h + m_g; }
	bool HasParent() const { return m_parent != nullptr; }
	bool IsClosed() const { return m_closed; } // Was added to the closed list?
	bool IsOpen() const { return m_open; } // Was added to the open list?
	void Close() { m_closed = true; m_open = false; }
	void Open() { m_open = true; m_closed = false; }
	void Reset()
	{
		m_parent = nullptr;
		m_h = 0.0f;
		m_g = 0.0f;
		m_closed = false;
		m_open = false;
	}

private:
	CWaypoint* m_my;
	CAStarNode* m_parent;
	float m_h;
	float m_g;
	bool m_closed;
	bool m_open;
};

/**
 * @brief Simple class for calculing search heuristic
*/
class IPathCostFunctor
{
public:

	float operator() (CAStarNode* to, CAStarNode* from)
	{
		if (from == nullptr)
		{
			return 0.0f; // starting node
		}
		else
		{
			CWaypoint* fromwpt = from->GetMyWaypoint();
			CWaypoint* towpt = to->GetMyWaypoint();
			float dist = fromwpt->DistTo(towpt);
			float cost = dist + from->GetG();
			return cost;
		}
	}
};


/**
 * @brief Abstract base class for performing an A* search;
 * @tparam CostFunc Heuristic cost functor
*/
template <typename CostFunc>
class IAStarSearch
{
public:
	virtual bool BuildPath(CWaypoint* start, CWaypoint* end, Vector* goal, CostFunc& costFunc) = 0;
	virtual void ResetSearch() = 0;
};

/**
 * @brief A Simple class for performaning an A star search
 * @tparam CostFunc Heuristic cost functor
*/
template <typename CostFunc>
class CSimpleAStarSearch : public IAStarSearch<CostFunc>
{
public:
	CSimpleAStarSearch();
	virtual ~CSimpleAStarSearch();

	virtual bool BuildPath(CWaypoint* start, CWaypoint* end, Vector* goal, CostFunc& costFunc);
	virtual void ResetSearch();
	std::vector<CWaypoint*>& GetPath();

private:
	CAStarNode* FindNodeInList(std::vector<CAStarNode*>* list, CWaypoint* waypoint);

	std::vector<CWaypoint*> m_path;
};

template<typename CostFunc>
inline CSimpleAStarSearch<CostFunc>::CSimpleAStarSearch()
{
}

template<typename CostFunc>
inline CSimpleAStarSearch<CostFunc>::~CSimpleAStarSearch()
{
}

template<typename CostFunc>
inline bool CSimpleAStarSearch<CostFunc>::BuildPath(CWaypoint* start, CWaypoint* end, Vector* goal, CostFunc& costFunc)
{
	if (start == nullptr)
		return false;

	if (end == nullptr)
		return false;

	if (start == end)
		return true;

	Vector actualGoal = goal ? *goal : end->GetPosition();
	CAStarNode* current = new CAStarNode(start);
	std::vector<CAStarNode*> m_openlist;
	std::vector<CAStarNode*> m_closedlist;


	current->SetH(start->DistTo(actualGoal));
	float initialCost = costFunc(current, nullptr);

	if (initialCost < 0.0f)
	{
		delete current;
		return false;
	}

	current->SetG(initialCost);
	current->Open();
	m_openlist.push_back(current);


	while (!m_openlist.empty())
	{
		// Step 1: Find an open node with the lowest F score
		float bestF = std::numeric_limits<float>::max();
		auto current_it = m_openlist.begin();
		current = *current_it;

		// The node with the lowest F cost from the open list	
		for (auto it = m_openlist.begin(); it != m_openlist.end(); it++)
		{
			auto opennodes = *it;

			if (opennodes->GetF() < current->GetF())
			{
				current = opennodes; // found a new open node with a better F score
				current_it = it;
			}
		}

		// Step 2: Remove said node from the open list and close it
		m_openlist.erase(current_it);
		m_closedlist.push_back(current);
		current->Close();

		if (current->GetMyWaypoint() == end)
			break;

		// Step 3: Generate a list of basenode's successors
		// Step 4: Loop each successor

		for (int i = 0; i < WaypointConst::MaxPaths; i++)
		{
			CWaypoint* nextwpt = current->GetMyWaypoint()->GetWaypointOfPath(i);

			if (nextwpt == nullptr)
				continue;

			if (FindNodeInList(&m_closedlist, nextwpt) != nullptr)
				continue;

			CAStarNode* successor = FindNodeInList(&m_openlist, nextwpt);

			if (successor == nullptr)
			{
				successor = new CAStarNode(nextwpt);
				float g = costFunc(successor, current);
				successor->SetG(g);
				successor->SetH(current->GetMyWaypoint()->DistTo(actualGoal));
				successor->SetParent(current);
				successor->Open();
				m_openlist.push_back(successor);
			}
			else
			{
				if (current->GetF() < successor->GetF())
				{
					successor->SetParent(current);
					successor->SetG(costFunc(successor, current));
				}
			}
		}
	}

	while (current != nullptr)
	{
		m_path.push_back(current->GetMyWaypoint());
		current = current->GetParent();
	}

	std::reverse(m_path.begin(), m_path.end());

	for (auto y : m_openlist)
	{
		delete y;
	}

	for (auto y : m_closedlist)
	{
		delete y;
	}

	m_openlist.clear();
	m_closedlist.clear();

	return true;
}

template<typename CostFunc>
inline void CSimpleAStarSearch<CostFunc>::ResetSearch()
{
}

template<typename CostFunc>
inline std::vector<CWaypoint*>& CSimpleAStarSearch<CostFunc>::GetPath()
{
	return m_path;
}

template<typename CostFunc>
inline CAStarNode* CSimpleAStarSearch<CostFunc>::FindNodeInList(std::vector<CAStarNode*>* list, CWaypoint* waypoint)
{
	for (auto node : *list)
	{
		if (node->GetMyWaypoint() == waypoint)
			return node;
	}

	return nullptr;
}

#endif // !WAYPOINT_BASE_PATH_FINDING_H_

