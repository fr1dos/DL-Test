#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <queue>
#include <set>

using namespace std;

struct RouteInfo {
    string numberoftrain;
    string departurestation;
    string arrivalstation;
    string cost;
    string departuretime;
    string arrivaltime;
};

int calculateTimeDifference(const string& departuretime, const string& arrivaltime) {

    istringstream isstrain1(departuretime);
    string hour1, minute1, sec1;
    getline(isstrain1, hour1, ':');
    getline(isstrain1, minute1, ':');
    getline(isstrain1, sec1, ':');

    int hourtrain1 = stoi(hour1);
    int minutetrain1 = stoi(minute1);
    int sectrain1 = stoi(sec1);

    istringstream isstrain2(arrivaltime);
    string hour2, minute2, sec2;
    getline(isstrain2, hour2, ':');
    getline(isstrain2, minute2, ':');
    getline(isstrain2, sec2, ':');

    int hourtrain2 = stoi(hour2);
    int minutetrain2 = stoi(minute2);
    int sectrain2 = stoi(sec2);

    int totalminutestrain1 = hourtrain1 * 60 + minutetrain1 + sectrain1 / 60;
    int totalminutestrain2 = hourtrain2 * 60 + minutetrain2 + sectrain2 / 60;

    if (totalminutestrain2 >= totalminutestrain1) return totalminutestrain2 - totalminutestrain1;
    return (24 * 60 - totalminutestrain1) + totalminutestrain2;
}

struct Route {
    vector<int> stations;
    int totalCost;
    int totalTime;
};

const string cst = "-1";
const int cst1 = -1;

Route findBestRoute(const vector<vector<string>>& minCosts, const vector<vector<int>>& timeDifferences, bool minimizeCost) {
    int numStations = minCosts.size();

    vector<int> stationsToVisit(numStations);
    for (int i = 0; i < numStations; i++) {
        stationsToVisit[i] = i;
    }

    Route bestRoute;
    bestRoute.totalCost = INT_MAX; 
    bestRoute.totalTime = INT_MAX; 


    do {
        double totalCost = 0;
        int totalTime = 0;
        bool validRoute = true;

        for (int i = 0; i < numStations - 1; i++) {
            int fromStation = stationsToVisit[i];
            int toStation = stationsToVisit[i + 1];

            if (minCosts[fromStation][toStation] == cst || timeDifferences[fromStation][toStation] == cst1) {
                validRoute = false;
                break;
            }

            totalCost += stod(minCosts[fromStation][toStation]);  
            totalTime += timeDifferences[fromStation][toStation];
        }

        if (!validRoute) continue;
        if ((minimizeCost && totalCost < bestRoute.totalCost) || (!minimizeCost && totalTime < bestRoute.totalTime)) {
            bestRoute.stations = stationsToVisit;
            bestRoute.totalCost = totalCost;
            bestRoute.totalTime = totalTime;
        }
        
    } while (next_permutation(stationsToVisit.begin(), stationsToVisit.end()));

    return bestRoute;
}

int main() {
    ifstream ip("test_task_data.csv");
    if (!ip.is_open()) {
        cerr << "ERROR: File Open" << '\n';
        return 1;
    }

    map<string, vector<RouteInfo>> graph;

    string numberoftrain;
    string departurestation;
    string arrivalstation;
    string cost;
    string departuretime;
    string arrivaltime;
    set<string> uniqueStations;

    while (ip.good()) {
        getline(ip, numberoftrain, ';');
        getline(ip, departurestation, ';');
        getline(ip, arrivalstation, ';');
        getline(ip, cost, ';');
        getline(ip, departuretime, ';');
        getline(ip, arrivaltime, '\n');

        RouteInfo route;
        route.numberoftrain = numberoftrain;
        route.departurestation = departurestation;
        route.arrivalstation = arrivalstation;
        route.cost = cost;
        route.departuretime = departuretime;
        route.arrivaltime = arrivaltime;
        uniqueStations.insert(departurestation);
        graph[departurestation].push_back(route);
    }

    ip.close();

    vector<string> stations(uniqueStations.begin(), uniqueStations.end());
    int num_stations = stations.size();
    

    vector<vector<string>> min_costs(num_stations, vector<string>(num_stations, cst));

    for (int i = 0; i < num_stations; i++) {
        for (int j = 0; j < num_stations; j++) {
            if (i != j) {
                string from_station = stations[i];
                string to_station = stations[j];

                string min_cost = cst;

                for (const RouteInfo& route : graph[from_station]) {
                    if (route.arrivalstation == to_station) {
                        if (min_cost == cst || route.cost < min_cost) {
                            min_cost = route.cost;
                        }
                    }
                }

                min_costs[i][j] = min_cost;
            }
        }
    }

    vector<vector<int>> timeDifferences(num_stations, vector<int>(num_stations, cst1));

    for (int i = 0; i < num_stations; i++) {
        for (int j = 0; j < num_stations; j++) {
            if (i != j) {
                string from_station = stations[i];
                string to_station = stations[j];

                int min_time_difference = INT_MAX;

                for (const RouteInfo& route : graph[from_station]) {
                    if (route.arrivalstation == to_station) {
                        int time_diff = calculateTimeDifference(route.departuretime, route.arrivaltime);
                        if (time_diff < min_time_difference) {
                            min_time_difference = time_diff;
                        }
                    }
                }
                    timeDifferences[i][j] = min_time_difference;
            }
        }
    }

    Route bestTimeRoute = findBestRoute(min_costs, timeDifferences, false);
    cout << "Best route in terms of time:" << endl;
    cout << "Number of train: " << graph[stations[bestTimeRoute.stations[0]]][0].numberoftrain << endl;

    for (int i = 0; i < num_stations - 1; i++) {
        int fromStation = bestTimeRoute.stations[i];
        int toStation = bestTimeRoute.stations[i + 1];
        string fromStationName = stations[fromStation];
        string toStationName = stations[toStation];
        string trainNumber;

        for (const RouteInfo& route : graph[fromStationName]) {
            if (route.arrivalstation == toStationName) {
                trainNumber = route.numberoftrain;
                break;
            }
        }

        cout << "from " << fromStationName << " by train " << trainNumber << " to " << toStationName << endl;
        cout << "Time: " << timeDifferences[fromStation][toStation] << " min" << endl;
    }

    cout << "Total travel time: " << bestTimeRoute.totalTime << " min" << endl;

    Route bestCostRoute = findBestRoute(min_costs, timeDifferences, true);
    cout << "The best route in terms of cost:" << endl;
    cout << "Number of train: " << graph[stations[bestCostRoute.stations[0]]][0].numberoftrain << endl;

    for (int i = 0; i < num_stations - 1; i++) {
        int fromStation = bestCostRoute.stations[i];
        int toStation = bestCostRoute.stations[i + 1];
        string fromStationName = stations[fromStation];
        string toStationName = stations[toStation];
        string trainNumber;

        for (const RouteInfo& route : graph[fromStationName]) {
            if (route.arrivalstation == toStationName) {
                trainNumber = route.numberoftrain;
                break;
            }
        }

        cout << "from " << fromStationName << " by train " << trainNumber << " to " << toStationName << endl;
        cout << "Cost: " << min_costs[fromStation][toStation] << endl;
    }

    cout << "Total cost: " << bestCostRoute.totalCost << endl;


    return 0;
}