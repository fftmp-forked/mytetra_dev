#!/usr/bin/env python

''' Generate test DB for load testing. By default generate ~7k branches and 108k records.'''

import os
import random
import shutil
import string
from dataclasses import dataclass
from lxml import etree


# default params
TREE_LEVEL = 3
BRANCHES_PER_LEVEL = 19
NOTES_PER_BRANCH = 15
DICT_FILE='./generateDictionary.txt'


@dataclass
class Range:
    '''approx limits in symbols'''
    min: int
    max: int

title_len = Range(25, 100)
tags_len = Range(10, 100)
note_len = Range(2000, 4000)
branch_name_len = Range(5, 20)


def get_id():
    '''@return random string to use as branch ID'''
    return ''.join(random.choice(string.ascii_lowercase + string.digits) for i in range(10))


class DbGenerator:
    '''main class'''

    def __init__(self, out_dir,
                 tree_level=TREE_LEVEL,
                 branches_per_level=BRANCHES_PER_LEVEL,
                 notes_per_branch=NOTES_PER_BRANCH):
        self.out_dir = out_dir
        self.branches_per_level = branches_per_level
        self.notes_per_branch = notes_per_branch
        self.branches_counter = 0

        # read dictionary file into memory
        with open(DICT_FILE, 'r', encoding='utf-8') as _f:
            self.dictionary = [line[:-1] for line in _f]

        # delete and create again output dir
        shutil.rmtree(out_dir, ignore_errors=True)
        os.makedirs(out_dir + '/base')

        root = etree.Element('root')
        etree.SubElement(root, 'format', version='1', subversion='2')
        tree = etree.SubElement(root, 'content')
        for child_branch in self.generate_branches(tree_level - 1):
            tree.append(child_branch)

        # write to disk
        etree.ElementTree(root).write(out_dir + '/mytetra.xml',
            pretty_print=True, xml_declaration=True, encoding='UTF-8',
            doctype='<!DOCTYPE mytetradoc>')

    def gen_text(self, len_range, delimeter=' '):
        '''Produces non-empty text. Result can be at max one word longer, than len_range.max'''
        desired_text_len = random.randint(len_range.min, len_range.max)
        text = random.choice(self.dictionary)
        while len(text) < desired_text_len:
            text += delimeter + random.choice(self.dictionary)
        return text

    def generate_branches(self, level):
        '''recursive'''
        res = []
        for _ in range(self.branches_per_level):
            self.branches_counter += 1
            branch_name = 'node' + str(self.branches_counter) + self.gen_text(branch_name_len)
            branch = etree.Element('node', name=branch_name, id=get_id())

            branch.append(self.generate_notes())
            if level > 0:
                for child_branch in self.generate_branches(level - 1):
                    branch.append(child_branch)
            res.append(branch)
        return res

    def generate_notes(self):
        ''' @TODO: create attaches. Now it present in metainfo, but absent on disk.'''
        notes = etree.Element('recordtable')

        for _ in range(self.notes_per_branch):
            dir_name = get_id()
            etree.SubElement(notes, 'record',
                             name=self.gen_text(title_len),
                             tags=self.gen_text(tags_len, ','),
                             id=get_id(), dir=dir_name, file='text.html')
            self.create_note_data(dir_name)

        return notes

    def create_note_data(self, dir_name):
        '''store one record'''
        dir_name = self.out_dir + '/base/' + dir_name
        os.makedirs(dir_name)
        with open(dir_name + '/text.html', 'w', encoding='utf-8') as _f:
            _f.write(self.generate_note_text())

    def generate_note_text(self):
        '''Generate simple one-sentence text.'''

        header = '''
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
<html><head>
<meta name="qrichtext" content="1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<style type="text/css">p, li { white-space: pre-wrap; }</style>
</head><body style=" font-family:'DejaVu Sans'; font-size:11pt; font-weight:400; font-style:normal;">
<p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
'''

        footer = '</p></body></html>'

        return header + self.gen_text(note_len) + footer


if __name__ == '__main__':
    DbGenerator(*os.sys.argv[1:])
