# License
Copyright @2017 The iMorpheusAI Authors
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 
http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

# Short distance track process 
This module includes three sub-modules.

## Data preprocess
This package assigns weights to GPS track that will be used ICP.
The track and weights come from long distance track process.

## Short distance track ICP
This module does the weighted ICP between lidar SLAM track and GPS track.

## Results merge
This package will merge all processed ENU tracks which are overlapped, so we will calculate weight average in overlap section.
