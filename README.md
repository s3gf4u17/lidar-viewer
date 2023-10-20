## Project

this prototype allows for quick preview of lidar point clouds.

## Find data

Fast and simple way to render data of your own choice.

<table width="100%">
  <tr>
  <td width="50.0%" align="center">Visit Polish Geoportal <a href="mapy.geoportal.gov.pl">here</a></td>
  <td width="50.0%" align="center">Enable download of lidar measurements</td>
  </tr>
  <tr>
  <td width="50.0%"><img src="/images/man1.png?raw=true"/></td>
  <td width="50.0%"><img src="/images/man2.png?raw=true"/></td>
  </tr>
  <tr>
  <td width="50.0%" align="center">Zoom in to find your tiles of interest</td>
  <td width="50.0%" align="center">Select desired density and download</td>
  </tr>
  <tr>
  <td width="50.0%"><img src="/images/man3.png?raw=true"/></td>
  <td width="50.0%"><img src="/images/man4.png?raw=true"/></td>
  </tr>
</table>

If your area of interest consists of more than one tile, you can connect them into one dataset using [LAStools](https://github.com/LAStools/LAStools). Simply download the source code, compile it and use command:

```bash
lasmerge -i <downloaded tiles>.laz -o <merged dataset>.laz
```

Keep in mind that lidar-viewer supports `.las` format only. To decompress data from `.laz` to `.las` you can use LAStools as well:

```bash
las2las -i <laz dataset>.laz -o <las dataset>.las
```

## Examples

<table width="100%">
  <tr>
  <td width="100.0%" align="center">Fragment of the Tatra National Park (57 049 110 points/1.9 GB)</td>
  </tr>
  <tr>
  <td width="100.0%"><img src="/images/scr1.png?raw=true"/></td>
  </tr>
  <tr>
  <td width="100.0%" align="center">Polsat Plus Arena Gdańsk (23 700 686 points/769 MB)</td>
  </tr>
  <tr>
  <td width="100.0%"><img src="/images/scr3.png?raw=true"/></td>
  </tr>
  <tr>
  <td width="100.0%" align="center">Jaworzno Power Station (60 298 174 points/2.0 GB)</td>
  </tr>
  <tr>
  <td width="100.0%"><img src="/images/scr2.png?raw=true"/></td>
  </tr>
</table>