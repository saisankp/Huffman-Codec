# <h1 align="center">Huffman Codec</h1>
<h2 align="center">By Prathamesh Sai</h2>
<img src="Huffman-Codec-Demo.gif" width="500" align="right"/>
<h3 align="center">What is a Codec?</h3>
<p align="center">A codec is a piece of software that can encode or decode a stream of data. This codec can compress plain text files using Huffman coding, and decode compressed files back to plain text.<br>
<h3 align="center">How it works.</h3>
<p>With Huffman Coding, first you get all possible characters with their frequencies, and find the characters with the lowest frequencies.
Then, you replace these characters with a pseudo-character pair representing them both,  whose frequency is the sum of the individual frequencies.
This process repeats until the list contains only one member. By doing this, you build a tree-like structure of nested pairs, with real characters at the leaves. To find the encoding of a character, you traverse the tree from the root to that character and record zero for a left branch and one for a right branch.</p> <br>
<p align="center">This results in a way to encode and decode any text you like! 😃</p>
