#!/usr/bin/env python

'''Retrieve all known public mytetra bases'''

import os
from dataclasses import dataclass
import zipfile

@dataclass
class db_credentials:
    name: str
    url: str



def download_file(url, out_filename):
    print('download: ' + url + '->' + out_filename)
    with requests.get(url) as r:
        with open(out_filename, 'w') as f:
            f.write(r.text)

def download_db(url, out_dir):
    download_file(url, out_dir + 'f.zip')
    with zipfile.ZipFile(path_to_zip_file, 'r') as zip_ref:
        zip_ref.extractall(directory_to_extract_to)

def download_all(out_dir):
    os.mkdir(out_dir)
    for db in BASES:
        os.mkdir(out_dir + '/' + db.name)
        download_db(db.url, out_dir + '/' + db.name)

if __name__ == '__main__':
    download_all(os.sys.argv[1])
