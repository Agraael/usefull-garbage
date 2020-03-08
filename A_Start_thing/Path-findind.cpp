#include <iostream>
#include <cmath>
#include <list>
#include <queue>
#include <vector>
#include <memory>
#include <functional>
#include <fstream>

struct Position
{
	int x{0};
	int y{0};
	int convertToIndex(int width, int height) {
		if (x >= width || y >= height || x < 0 || y < 0)
			return -1;
		return (y * width) + x;
	}

	bool operator==(Position const& rhs) const{
		return std::tie(x, y) == std::tie(rhs.x, rhs.y);
	}
};

struct Node
{
	Position pos{0,0};
	double globalGoal{INFINITY};
	std::shared_ptr<Node> parent{nullptr};

	std::vector<std::pair<Position, int>> findNeighbours(unsigned char const* pMap, int nMapWidth, int nMapHeight) {
		std::vector<std::pair<Position, int>> positions = {{Position{pos.x, pos.y - 1}, -1},
			{Position{pos.x, pos.y + 1}, -1},
			{Position{pos.x - 1, pos.y}, -1},
			{Position{pos.x + 1, pos.y}, -1}};

		for (auto& position : positions) {
			auto index = position.first.convertToIndex(nMapWidth, nMapHeight);
			if (index != -1)
				position.second = index;
		}
		return std::move(positions);
	}
};
using NodePtr = std::shared_ptr<Node>;

struct CompNode{
	bool operator()(NodePtr const& a, NodePtr const& b){
		return a->globalGoal > b->globalGoal;
	}
};



int FindPath(int const nStartX, int const nStartY,
             int const nTargetX, int const nTargetY,
             unsigned char * pMap, int const nMapWidth, int const nMapHeight,
             int* pOutBuffer, int const nOutBufferSize)
{
	static auto distance = [](Position const& A, Position const& B){
		return sqrtf(powf(A.x - B.x, 2) * powf(A.y - B.y, 2));};
	static auto heuristic = [](Position const& A, Position const& B){
		return 0.25 * (std::abs(A.x - B.x) + std::abs(A.y - B.y));};

	NodePtr startNode = std::make_shared<Node>(Node({.pos = {nStartX, nStartY}}));
	Position targetPostion = {nTargetX, nTargetY};
	NodePtr currentNode{nullptr};
	NodePtr endNode{nullptr};

	std::priority_queue<NodePtr, std::vector<NodePtr>, CompNode> NotTestedNodes{};
	std::vector<std::pair<bool, float>> positionsMap(nMapWidth * nMapHeight, {false, INFINITY});
	positionsMap[startNode->pos.convertToIndex(nMapWidth, nMapHeight)].second = 0.0f;

	startNode->globalGoal = distance(startNode->pos, targetPostion);
	NotTestedNodes.push(startNode);
	while(!NotTestedNodes.empty()) {
		currentNode = NotTestedNodes.top();
		positionsMap[currentNode->pos.convertToIndex(nMapWidth, nMapHeight)].first = true;
		pMap[currentNode->pos.convertToIndex(nMapWidth, nMapHeight)] = 2;
		NotTestedNodes.pop();
		if (currentNode->pos == targetPostion) {
			endNode = currentNode;
			break;
		}
		for (auto& neighbourPos : currentNode->findNeighbours(pMap, nMapWidth, nMapHeight)) {
			if (neighbourPos.second == -1 || pMap[neighbourPos.second] == 0)
				continue;
			auto possiblyLowerGoal = positionsMap[currentNode->pos.convertToIndex(nMapWidth, nMapHeight)].second + 1;
			Node newNode{.pos = neighbourPos.first};
			if (possiblyLowerGoal < positionsMap[neighbourPos.second].second) {
				newNode.parent = currentNode;
				positionsMap[neighbourPos.second].second = possiblyLowerGoal;
				newNode.globalGoal = possiblyLowerGoal + heuristic(neighbourPos.first, targetPostion);
			}
			if(!positionsMap[neighbourPos.second].first)
				NotTestedNodes.push(std::make_shared<Node>(newNode));
		}
	}
	if (endNode == nullptr)
		return -1;
	std::function<int(NodePtr const&, int**, int, bool*)> fillOutBuffer;
	bool err{false};
	fillOutBuffer = [&fillOutBuffer, nMapWidth, nMapHeight](NodePtr const& node, int** pOutBuffer, int nOutBufferSize, bool* err) {
		if (nOutBufferSize <= 0)
			*err = true;
		if (node->parent == nullptr || *err)
			return 0;
		auto lenght = 1 + fillOutBuffer(node->parent, pOutBuffer, nOutBufferSize - 1, err);
		**pOutBuffer = node->pos.convertToIndex(nMapWidth, nMapHeight);
		*pOutBuffer = *pOutBuffer + 1;
		return lenght;
	};
	auto lenght = fillOutBuffer(endNode, &pOutBuffer, nOutBufferSize, &err);
	return err ? -1 : lenght;
}

void printResult(int const nStartX, int const nStartY,
                 int const nTargetX, int const nTargetY,
                 unsigned char const* pMap, int const nMapWidth, int const nMapHeight,
                 int* pOutBuffer, int const nOutBufferSize)
{
	auto j = 0;
	auto func = [pOutBuffer, nOutBufferSize](int nb) {
		for(int i = 0; i < nOutBufferSize; i++) {
			if (nb == pOutBuffer[i])
				return true;
		}
		return false;
	};

	for (int i = 0; i < nMapWidth * nMapHeight; ++i) {
		if (i % nMapWidth == 0)
			std::cout << std::endl;
		if (i == nStartX + nStartY * nMapWidth)
			std::cout << 'O';
		else if (i == nTargetX + nTargetY * nMapWidth)
			std::cout << 'X';
		else if (func(i)) {
			std::cout << 'o';
			j++;
		}
		else if (pMap[i] == 2)
			std::cout << '.';
		else if (pMap[i] == 0)
			std::cout << 'T';
		else
			std::cout << ' ';


	}
}

int main() {
	std::ifstream ifs ("./Maze", std::ifstream::in);

	unsigned char pMap[73 * 23];
	char c = ifs.get();
	int j = 0;

	Position start;
	Position target;
	while (ifs.good()) {
		if (c != '\n') {
			if (c == 'O') {
				start.x = j % 73;
				start.y = j / 73;
			}
			if (c == 'X') {
				target.x = j % 73;
				target.y = j / 73;
			}
			pMap[j] = c == '1' ? 0 : 1;
			j++;
		}

		c = ifs.get();
	}

	ifs.close();
	std::cout << target.x << "-" << target.y << std::endl;
	int nOutBufferSize{1000};
	int pOutBuffer[nOutBufferSize];
	std::cout << "begin" << std::endl;
	auto res = FindPath(start.x, start.y, target.x, target.y, pMap, 73, 23, pOutBuffer,nOutBufferSize);
	std::cout << "end" << std::endl;
	std::cout << res << std::endl;
	for (int i = 0; i < res; i++) {
		std::cout << pOutBuffer[i];
		if (i+1 < res)
			std::cout << " -> ";
	}
	std::cout << std::endl;
	printResult(start.x, start.y, target.x, target.y, pMap, 73, 23, pOutBuffer,res);
	return 0;
}