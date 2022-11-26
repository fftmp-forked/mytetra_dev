#!/usr/bin/env python
'''
Сервисный скрипт GetBase - выкачиватель дерева в формате MyTetra
Выкачивает формат данных 1.1, т.е. пока не поддерживает прикрепляемые файлы и иконки
'''

import os
import sys
import shutil
from lxml import etree
import requests

def download_file(url, out_filename):
    print('download: ' + url + '->' + out_filename)
    with requests.get(url) as r:
        with open(out_filename, 'w') as f:
            f.write(r.text)

def download_db(start_url, out_dir):
    if os.path.exists(out_dir):
        sys.exit('Error: out dir already exist')
    os.makedirs(out_dir)

    (base_url, mytetra_filename) = start_url.rsplit('/', 1)
    if mytetra_filename != 'mytetra.xml':
        sys.exit('Error: url must point to mytetra.xml file')
    download_file(start_url, out_dir + mytetra_filename)

    tree = etree.parse(out_dir + mytetra_filename).getroot().find('content')
    download_recurse(tree, base_url, out_dir)


def download_recurse(element, base_url, out_dir):
    node_id = element.get('id')

    record_table_element = element.find('recordtable')

    # download note records
    if record_table_element is not None:
        for record in record_table_element.findall('record'):
            download_record(record, base_url, out_dir)

    # recursive call
    for node in element.findall('node'):
        download_recurse(node, base_url, out_dir)


def download_record(record, base_url, out_dir):
    record_id = record.get('id')
    record_dir = record.get('dir')
    record_file_name = record.get('file')

    print('mkdir 4file' + out_dir + '/base/' + record_dir)
    os.makedirs(out_dir + '/base/' + record_dir)
    rel_path = '/base/' + record_dir + '/' + record_file_name
    recod_file_url = base_url + rel_path

    download_file(base_url + rel_path, out_dir + rel_path)


    #// Выкачиваются картинки, принадлежащие записи
    #downloadImages($recordDir, $recodFileNameForSave);


'''
  function downloadImages($recordDir, $recodFileName)
  {
    global $mytetraUrl, $recordsDir;
    $rootUrl=substr($mytetraUrl, 0, -12);

    // Получение DOM-дерева файла записи
    $dom = new DomDocument;
    if( !(@$dom->load($recodFileName)) )
    {
      echo "Bad HTML/XML file ".$recodFileName.", images not download."."\n";
      return;
    }

    // Элементы с картинками
    $imagesElements=$dom->documentElement->getElementsByTagName("img");

    foreach($imagesElements as $imageElement)
    {
      $imgFileName=$imageElement->getAttribute("src");

      if(substr($imgFileName,0,4)==="http")
      {
        echo "Direct link to external image: ".$imgFileName.", Image discard."."\n";
        continue;
      }

      $imgUrl=$rootUrl."/base/".$recordDir."/".$imgFileName;

      $imgFileNameForSave=$recordsDir."/".$recordDir."/".$imgFileName;

      // Скачивание файла с картинкой
      $text=@file_get_contents($imgUrl);
      if($text===FALSE)
      {
        echo "Can't download image file ".$imgUrl."\n";
        return;
      }

      echo "Image: ".$imgFileNameForSave."\n";

      // Сохранение картинки в файл
      $file=fopen( $imgFileNameForSave, 'wb');
      fwrite($file, $text);
      fclose($file);
    }
  }
'''

if __name__ == '__main__':
    download_db(os.sys.argv[1], os.sys.argv[2])
