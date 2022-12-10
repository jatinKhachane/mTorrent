# mTorrent
 
* Implemented a bit‑torrent like file sharing application using concepts from operating systems and multithreading.
* The entire projects is based on two major part i.e tracker and peer, where tracker keeps the information about which peer is having which data
and how much data.
* Unlike Single Server file storage, here when peer request for the file, tracker responds with list of peers sharing that file and requesting peer can download file from all available peer rather than single server
* The downloading of the file is handled in the form of chunks


* Parallel Download algorithm:
Create a 2-D map of chunk index - list of peers having that chunk of the file
For each chunk index i:
    Generate a random number x of range (0, len(peer_list_of_chunk))
    Download chunk i from the peer present at index x
