/*
 * Account.h
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include "Access.h"
#include "Tenant.h"
#include "AuthenticationMethod.h"
#include <iostream>
#include <vector>

namespace Swift {

/** Associated roles with this account **/
struct Role;

class Account {
private:

  /**
   * ObjectStore tenant
   */
  Tenant *tenant;

  /**
   * The ObjectStore username
   */
  std::string username;

  /**
   * The ObjectStore password
   */
  std::string password;

  /**
   * The ObjectStore authentication URL (Keystone)
   */
  std::string authUrl;

  /**
   * ObjectStore tokens expire after 24 hours. If reauthentication is allowed (default behaviour),
   * after expiration, Account will reauthenticate itself and get a new authentication token. If
   * reauthentication is turned off, you will have to manually arrange reauthentication. This mode
   * is recommended for web servers or otherwise long-living processes.
   */
  bool allowReauthenticate = true;

  /**
   * Public url to interact with ObjectStore
   */
  std::string publicHost;

  /**
   * The preferred region will determine what Swift end-point will be chosen. If no preferred region is
   * set (default), the first applicable end-point will be selected.
   */
  std::string preferredRegion;

  /**
   * The delimiter is used to check for directory boundaries. The default will be a '/'.
   */
  char delimiter = '/';

  /**
   * The method of authentication. Various options:
   * <ul>
   *     <li>
   *         <b>BASIC</b>; authenticate against Swift itself. Authentication URL, username and password
   *         must be passed.
   *     </li>
   *     <li>
   *         <b>TEMPAUTH</b>; authenticate against Swift itself. Authentication URL, username and password
   *         must be passed.
   *     </li>
   *     <li>
   *         <b>KEYSTONE</b> (default); makes use of OpenStack Compute. Authentication URL, username and
   *         password must be passed. Ideally, tenant ID and/or name are passed as well. API can auto-
   *         discover the tenant if none is passed and if it can be resolved (one tenant for user).
   *     </li>
   * </ul>
   */
  AuthenticationMethod authenticationMethod = AuthenticationMethod::KEYSTONE;

public:
  virtual ~Account();
  Account();

  /**
   * Trigger the authentication against Object Store. There are two use cases for this method. The first is
   * triggered pro-actively by the user by calling authenticate on the client. The second is when the token
   * has expired and AbstractSecureCommand triggers a re-authentication.
   * @return the access element including a new token
   */
  Access authenticate();

  /**
   * Force the Account to reload its metadata
   */
  void reload();

  /**
   * The number of bytes stored by the StoredObjects in all Containers in the Account.
   * @return number of bytes
   */
  long getBytesUsed();

  /**
   * The number of StoredObjects in all Containers in the Account.
   * @return number of StoredObjects
   */
  int getObjectCount();

  /**
   * Returns the server time in milliseconds since 1970
   * @return server time
   */
  long getServerTime();

  /**
   * Compares the local time to the server time and maintains a number of milliseconds to account for the
   * difference. This number is taken into account when an absolute expiry time is passed to the server.
   */
  void synchronizeWithServerTime();

  /**
   * Calculate the server time taking into account the number of seconds passed. This method makes use
   * of the server time modifier which is calculated in synchronizeWithServerTime().
   * @param seconds number of seconds after the actual server time
   * @return calculated server time, including the number of seconds in the future
   */
  long getActualServerTimeInSeconds(long seconds);

  /**
   * An ObjectStore authentication token will expire after 24 hours. In a long-living web application,
   * the account instance may exist longer than that. If reauthentication is allowed, a new token will
   * be fetched once the existing one has expired.
   * @param allowReauthenticate whether reauthentication is allowed
   * @return instance of Account
   */
  Account setAllowReauthenticate(bool allowReauthenticate);

  /**
   * Set your own host to prefix the URLs of objects
   * @param host host of an object's URL
   * @return instance of Account
   */
  Account setPublicHost(const std::string &host);

  /**
   * Set your own private host to prefix the URLs of objects
   * @param host host of an object's private URL
   * @return instance of Account
   */
  Account setPrivateHost(const std::string &host);

  /**
   * If ContainerCaching is enabled, Account will keep track of its Container instances and reuse them.
   * @param containerCaching whether container caching is allowed
   */
  Account setAllowContainerCaching(bool containerCaching);

  /**
   * Saves the password to the Account. The password will be used to create server side hashes. This is required for
   * TempURL (both GET and PUT). The server will match a generated hash against the hash that is passed in a
   * tempURL. If identical, it passed the first test. Note that if password is not set, TempURLs will not work.
   * Note that the password saved is the one set either in AccountConfig, or set with Account.setHashPassword(std::string).
   * @param hashPassword the password to use for generating the hashes
   */
  Account setHashPassword(const std::string &hashPassword);

  /**
   * Returns the hash password originally set on Account.
   * @return hash password
   */
  const std::string getHashPassword();

  /**
   * Empties the Container Cache
   */
  void resetContainerCache();

  /**
   * Whether reauthentication on expiration of the authentication token is allowed, or this is done
   * manually by the client
   * @return whether reauthentication is allowed
   */
  bool isAllowReauthenticate();

  /**
   * Increase the call counter which tracks how many calls are made to the ObjectStore
   */
  void increaseCallCounter();

  /**
   * Returns the number of HTTP calls that have been made to the ObjectStore server. This could be
   * useful to check the efficiency of the methods and configuration in use
   * @return number of calls made to the ObjectStore server
   */
  int getNumberOfCalls();

  /**
   * Returns the original host, regardless of the configured and private host settings
   * @return original host
   */
  std::string getOriginalHost();

  /**
   * Returns the tenants of the account. Note that this is the only Account method that interacts with the object
   * store and will work when no tenant has been set. All others will throw an exception.
   * @return the tenants belonging to the Account
   */
  std::vector<Tenant> getTenants();

  /**
   * Checks whether a tenant ID and/or name were supplied. If not, the account can only be used to retrieve
   * a list of tenants.
   */
  bool isTenantSupplied();
};

} /* namespace Swift */
#endif /* ACCOUNT_H_ */
