#!/bin/bash
wget https://artifacts.crowdin.com/repo/deb/crowdin3.deb -O /tmp/crowdin.deb
sudo dpkg -i /tmp/crowdin.deb

crowdin upload sources
crowdin download