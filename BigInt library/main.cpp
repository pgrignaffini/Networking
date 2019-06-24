#include "Big_Int.hpp"

int main()
{

  int bandwidth, bandwidth_req, n_users, target_users;
  float prob_inactive, prob_active;

  cout << "Insert bandwidth: ";
  cin >> bandwidth;
  cout << endl;
  cout << "Insert bandwidth requirement of each user: ";
  cin >> bandwidth_req;
  cout << endl;
  cout << "Insert number of users: ";
  cin >> n_users;
  cout << endl;
  cout << "Insert probability that an user is inactive: ";
  cin >> prob_inactive;
  cout << endl;
  cout << "Insert target number of users: ";
  cin >> target_users;
  cout << endl;


  if (prob_inactive >= 1) prob_inactive /= 100;
  prob_active = 1 - prob_inactive;

  //cout << "PA " << prob_active << endl;
  //cout << "PI " << prob_inactive << endl;

  Big_Int bigBinomial = Big_Int::binomialCoeff(n_users,target_users);
  //cout << "BIN " << bigBinomial << endl;

  int binomial = Big_Int::Big2Int(bigBinomial);

  float firstFactor = pow(prob_active,target_users);
  //cout << "first " << firstFactor << endl;
  float secondFactor = pow(prob_inactive,(n_users - target_users));
  //cout << "second " << secondFactor << endl;

  float result = binomial * firstFactor * secondFactor;
  cout << result << endl;

  //cout << Big_Int::binomialCoeff(a,b) << endl;
  //cout << Big_Int::factorial(Big_Int(100)) << endl;

  return 0;
}
