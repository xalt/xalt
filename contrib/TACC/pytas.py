#!/usr/bin/env python
# -*- coding: utf-8 -*-
import json
import os
import re
import requests
from requests.auth import HTTPBasicAuth
from suds.client import Client as Suds

"""
Client class for the TAS REST APIs.
"""
class client:

    """
    Instantiate the API Object with a base URI and service account credentials.
    The credentials should be a hash with keys `username` and `password` for
    BASIC Auth.
    """
    def __init__(self, baseURL = None, credentials = None):
        if (baseURL == None):
            baseURL = os.environ.get('TAS_URL')

        if (credentials == None):
            key = os.environ.get('TAS_CLIENT_KEY')
            secret = os.environ.get('TAS_CLIENT_SECRET')
            credentials = {'username':key, 'password':secret}

        self.baseURL = baseURL
        self.credentials = credentials
        self.auth = HTTPBasicAuth(credentials['username'], credentials['password'])

    """
    Authenticate a user
    """
    def authenticate(self, username, password):
        payload = {'username': username, 'password': password}
        headers = { 'Content-Type':'application/json' }
        r = requests.post(self.baseURL + '/auth/login', data=json.dumps( payload ), auth=self.auth, headers=headers)
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception('Authentication Error', resp['message'])

    """
    Users
    """
    def get_user(self, id = None, username = None):
        if id:
            url = '{0}/v1/users/{1}'.format(self.baseURL, id)
        elif username:
            url = '{0}/v1/users/username/{1}'.format(self.baseURL, username)
        else:
            raise Exception('username or id is required!')

        r = requests.get(url, auth=self.auth);
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception('User not found', resp['message'])

    def save_user(self, id, user):
        if id:
            url = '{0}/v1/users/{1}'.format( self.baseURL, id )
            method = 'PUT'
        else:
            url = '{0}/v1/users'.format( self.baseURL )
            method = 'POST'

        headers = { 'Content-Type':'application/json' }
        r = requests.request( method, url, data=json.dumps( user ), auth=self.auth, headers=headers )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            if id:
                raise Exception( 'Unable to save user id={0}'.format( id ), resp['message'] )
            else:
                raise Exception('Unable to save new user', resp['message'])

    def verify_user(self, id, code):
        url = '{0}/v1/users/{1}/{2}'.format( self.baseURL, id, code )
        r = requests.put( url, auth=self.auth )
        resp = r.json()
        if resp['status'] == 'success':
            return True
        else:
            raise Exception( 'Error verifying user id={0}'.format( id ), resp['message'] )

    def request_password_reset( self, username, source=None ):
        if source:
            url = '{0}/v1/users/{1}/passwordResets?source={2}'.format( self.baseURL, username, source )
        else:
            url = '{0}/v1/users/{1}/passwordResets'.format( self.baseURL, username )
        r = requests.post( url, auth=self.auth )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception( 'Error requesting password reset for user={0}'.format( username ), resp['message'] )

    def confirm_password_reset( self, username, code, new_password, source=None  ):
        if source:
            url = '{0}/v1/users/{1}/passwordResets/{2}?source={3}'.format( self.baseURL, username, code, source )
        else:
            url = '{0}/v1/users/{1}/passwordResets/{2}'.format( self.baseURL, username, code )
        body = {
            'password': new_password
        }
        headers = { 'Content-Type':'application/json' }
        r = requests.post( url, data=json.dumps( body ), auth=self.auth, headers=headers )
        if r.status_code == 200:
            resp = r.json()
            print resp
            if resp['status'] == 'success':
                return True
            else:
                raise Exception( 'Failed password reset for user={0}'.format( username ), resp['message'] )
        else:
            raise Exception( 'Failed password reset for user={0}'.format( username ), 'Server Error' )

    """
    Data Lists
    Institutions/Departments
    """
    def institutions(self):
        url = re.sub(r'/api[\-a-z]*$', '/TASWebService/PortalService.asmx?wsdl', self.baseURL)
        api = Suds(url, username=self.credentials['username'], password=self.credentials['password'])
        resp = api.service.GetInstitutions()
        institutions = []
        for i in resp.Institution:
            if i.Validated:
                inst = {
                    'id': i.ID,
                    'name': i.Name,
                    'active': i.Selectable,
                    'children': self._get_departments(i)
                }

                institutions.append(inst)

        return institutions

    def _get_departments(self, institution):
        depts = []

        if institution.Children:
            for child in institution.Children.Institution:
                depts.append({
                    'id': child.ID,
                    'name': child.Name,
                    'active': child.Selectable,
                    'children': []
                })
                if child.Children:
                    depts.extend(self._get_departments(child))

        return depts

    def get_institution(self, institution_id):
        url = '{0}/v1/institutions/{1}'.format( self.baseURL, institution_id )

        headers = { 'Content-Type':'application/json' }

        r = requests.get( url, auth=self.auth, headers=headers )
        if r.status_code == 200:
            resp = r.json()
            if resp['status'] == 'success':
                inst = {
                    'id': resp['result']['id'],
                    'name': resp['result']['name'],
                    'children': self._departments(resp['result']['departments'])
                }

                return inst
            else:
                raise Exception( 'Failed to fetch institution for id={0}'.format( institution_id ), resp['message'] )
        else:
            raise Exception( 'Failed to fetch institution for id={0}'.format( institution_id ), 'Server error' )

    def get_departments(self, institution_id):
        url = '{0}/v1/institutions/{1}/departments'.format( self.baseURL, institution_id )

        headers = { 'Content-Type':'application/json' }

        r = requests.get( url, auth=self.auth, headers=headers )
        if r.status_code == 200:
            resp = r.json()
            if resp['status'] == 'success':
                return self._departments(resp['result'])

    def get_department(self, institution_id, department_id):
        departments = self.get_departments(institution_id)

        for dept in departments:
            if dept['id'] == department_id:
                return dept

        return None


    def _departments(self, departments):
        depts = []

        for dept in departments:
            depts.append({
                'id': dept['id'],
                'name': dept['name']
            })

        return depts

    def countries(self):
        url = re.sub(r'/api[\-a-z]*$', '/TASWebService/PortalService.asmx?wsdl', self.baseURL)
        api = Suds(url, username=self.credentials['username'], password=self.credentials['password'])
        resp = api.service.GetCountries()
        countries = []

        for c in resp.Country:
            countries.append({
                'id': c.ID,
                'name': c.Name
            })

        return countries

    """
    Fields
    """
    def fields( self ):
        r = requests.get( '{0}/tup/projects/fields'.format(self.baseURL), auth=self.auth )
        resp = r.json()
        return resp[ 'result' ]

    """
    Projects
    """
    def projects( self ):
        headers = { 'Content-Type':'application/json' }
        r = requests.get( '{0}/v1/projects/group/Chameleon'.format(self.baseURL), headers=headers, auth=self.auth )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception('Projects not found', resp['message'])

    def project( self, id ):
        headers = { 'Content-Type':'application/json' }
        r = requests.get( '{0}/v1/projects/{1}'.format(self.baseURL, id), headers=headers, auth=self.auth )
        resp = r.json()
        return resp['result']

    def projects_for_user( self, username ):
        headers = { 'Content-Type':'application/json' }
        r = requests.get( '{0}/v1/projects/username/{1}'.format(self.baseURL, username), headers=headers, auth=self.auth )
        resp = r.json()
        return resp['result']

    """
    Project is a dict with:
    {
        'title': string, # project title
        'typeId': number, # project type; 0=Research, 2=Startup
        'description': string, # project abstract
        'source': string, # project source, e.g. Chameleon
        'fieldId': number, # project field of science
        'piId': number, # PI user ID
        'allocations': [ # optional list of requested allocations
            {
                'resourceId': number, # resource ID
                'requestorId': number, # user ID making request
                'justification': string,
                'dateRequested': datetime,
                'computeRequested': number # SUs
            },
        ]
    }
    """
    def create_project( self, project ):
        url = '{0}/v1/projects'.format( self.baseURL )
        headers = { 'Content-Type':'application/json' }
        r = requests.post( url, data=json.dumps( project ), auth=self.auth, headers=headers )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception( 'Failed to create project', resp['message'] )

    def edit_project( self, project ):
        url = '{0}/v1/projects/{1}'.format( self.baseURL, project['id'] )
        headers = { 'Content-Type':'application/json' }
        r = requests.put( url, data=json.dumps( project ), auth=self.auth, headers=headers )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception( 'Failed to update project', resp['message'] )

    def edit_allocation( self, allocation ):
        url = '{0}/v1/allocations/{1}'.format( self.baseURL, allocation['id'] )
        headers = { 'Content-Type':'application/json' }
        r = requests.put( url, data=json.dumps( allocation ), auth=self.auth, headers=headers )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception( 'Failed to update allocation', resp['message'] )

    """
    Project Users
    """
    def get_project_users( self, project_id ):
        r = requests.get( '{0}/v1/projects/{1}/users'.format( self.baseURL, project_id ), auth=self.auth )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception( 'Failed to get project users', resp['message'] )

    def add_project_user( self, project_id, username ):
        r = requests.post( '{0}/v1/projects/{1}/users/{2}'.format( self.baseURL, project_id, username ), auth=self.auth )
        resp = r.json()
        if resp['status'] == 'success':
            return True
        else:
            raise Exception( 'Failed to add user to project', resp['message'] )

    def del_project_user( self, project_id, username ):
        r = requests.delete( '{0}/v1/projects/{1}/users/{2}'.format( self.baseURL, project_id, username ), auth=self.auth )
        resp = r.json()
        if resp['status'] == 'success':
            return True
        else:
            raise Exception( 'Failed to remove user from project', resp['message'] )
    """
    Allocation
    """
    def allocation_approval(self, id, allocation):
        url = '{0}/v1/allocations/{1}'.format( self.baseURL, id )
        method = 'PUT'
        headers = { 'Content-Type':'application/json' }
        r = requests.request( method, url, data=json.dumps( allocation ), auth=self.auth, headers=headers )
        resp = r.json()
        if resp['status'] == 'success':
            return resp['result']
        else:
            raise Exception( 'Unable to process allocation approval for allocation id:'.format( id ), resp['message'] )
