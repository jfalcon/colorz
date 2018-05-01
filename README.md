# colorz

Colorz - The RGB Assistant

## Description

After developing applications and web pages for a while, I came to a point where I realized a major gap exists in the typical programming paradigm in that visualizing fine-grained color values in code was almost impossible unless you happen to use a WYSIYYG or RAD environment. And that there are still plenty of circumstances where such an environment will still be used.

Even the notion of HTML's naming colors never really helped because it only applied to basic values (i.e., <body bgcolor="white">), or had some obscure name that nobody even knows what it is. Lastly, most developers already know the basic colors, like RGB 255, 255, 255 equals white, RGB 0, 255, 255 equals cyan, etc.

This still begs the question, what if you want to see a color, perhaps coordinate a color in an environment-independent manner that was quick to use and allowed for easy copy and pasting for a pseudo integration mechanism. And, regardless of my editor of choice I would always have a means to do this. Thus, Colorz was born.

What Colorz is, is a tiny program that allows you to use three scrollbars to specify colors in the RGB color space by manipulating the R intensity, the G intensity, and the B intensity of a color while showing you what it looks like and the appropriate values, essentially allowing you to see a color in real-time while you choose it.

Although Colorz was one of the first applications of such a nature out there on the web, realistically, the idea for it is not exclusive to me. As such, my goal was to focus in on just what it is Colorz is supposed to be. With this in mind, the idea was to keep it small and lightweight to deliver just what's needed and nothing more. It has a smaller memory footprint than Notepad so it will not hog system resources.

## Specifications

### Mode

Switch between foreground (text color) and background modes. This enables you to coordinate color values and visually see what color looks good with what color.

### Format

| Option | Description |
| ------ | ------ |
| Decimal | This option allows you to specify the RGB color in decimal notation, with values between 0 and 255 for each intensity (suitable for environments such as the Windows API). |
| Float | This option allows you to specify the RGB color in floating point notation, with values between 0.0 and 1.0 for each intensity (suitable for environments such as OpenGL). |
| Hexe | This option allows you to specify the RGB color in hexadecimal notation, with values between 0 and FF for each intensity (suitable for environments such as HTML). |
| Grayscale | This option keeps the scrollbars in uniform to allow only shades of gray. |
| On Top | This option will keep the Colorz window on top of other windows for convenient placement while you are working. |
| Grab Color | This option is probably the reason most people use this application. It will allow you to pick the RGB color off of any point on the screen using the cursor. This can be a huge timesaver if you find the perfect color and want to know the values for it. After turning this option on, hold down the CTRL key to activate it. |
| Auto Copy | This feature allows you to paste the value directly in your code! Turning this on will have whatever value you specify (or pulled from the screen) be automatically copied to the clipboard. |

### Features

* Has a small memory footprint, and will not hog system resources.
* Highlights the textual output in correspondence to the Auto Copy selection.
* Allows you to use thee different ways to specify a color; so regardless of the environment, Colorz will work with it.

### Fixes

* The "Grab Color" activation hotkey was changed to CTRL to avoid conflicts with menus and accelerators.
* Now, when switching between background and foreground mode, Colorz retains the separate values.
* When finished grabbing a color, the scrollbars automatically reflect the new color.
* Must hold down the ALT key for the "Grab Color" feature to work so moving the cursor again won't overwrite selections.

This program has made my life a little easier numerous times. If it helps me, I hope it'll help someone else out there in cyberspace. And maybe, some will learn from it. Enjoy!
