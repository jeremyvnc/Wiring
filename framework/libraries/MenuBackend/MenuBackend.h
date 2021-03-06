/* $Id: MenuBackend.h 1231 2011-08-25 10:57:49Z abrevig $
||
|| @author         Alexander Brevig <abrevig@wiring.org.co>
|| @url            http://wiring.org.co/
|| @contribution   Adrian Brzezinski
|| @contribution   Bernhard Benum
|| @contribution   Brett Hagman <bhagman@wiring.org.co>
|| @contribution   Ryan Michael <kerinin@gmail.com>
|| @contribution   Jeremy Vance <jeremyvnc@gmail.com>
||
|| @description
|| | Provides an easy way of making menus.
|| |
|| | Wiring Cross-platform Library
|| #
||
|| @license Please see cores/Common/License.txt.
||
*/

#ifndef MENUBACKEND_H
#define MENUBACKEND_H

class MenuBackend; //forward declaration of the menu backend
class MenuItem; //forward declaration of the menu item

struct MenuChangeEvent
{
  const MenuItem &from;
  const MenuItem &to;
};

struct MenuUseEvent
{
  MenuItem &item;
};

struct MenuItemChangeEvent
{
  const MenuItem &item;
};

struct MenuMoveEvent
{
  const MenuItem &item;
};

typedef void (*cb_change)(MenuChangeEvent);
typedef void (*cb_use)(MenuUseEvent);
typedef void (*cb_item_change)(MenuItemChangeEvent);
typedef void (*cb_move)(MenuMoveEvent);


/*
  A menu item will be a container for an item that is a part of a menu
  Each such item has a logical position in the hierarchy as well as a text and maybe a mnemonic shortkey
*/
class MenuItem
{
  public:
    /*
    || @constructor
    || | Basic item with a name and an optional mnemonic  [File (f)]
    || #
    ||
    || @example
    || | MenuItem file = MenuItem("File",'F');
    || #
    ||
    || @parameter itemName  the name of the item
    || @parameter shortKey the mnemonic 'shortkey'
    */
    MenuItem(const char* itemName, char shortKey = '\0') : name(itemName), shortkey(shortKey)
    {
      before = right = after = left = 0;
      level = value = 0;
      menuBackend = 0;
    }

    /*
    || @constructor
    || | A complex item that will use utem change events
    || | It has a name, an optional level indicator (which level in the hierachy, this value is handy for jumping streches in the structure), and an optional mnemonic
    || #
    ||
    || @example
    || | MenuItem file = MenuItem(menuBackend, "File", 1, 'F');
    || #
    ||
    || @parameter mb  the menu backend that controls this item
    || @parameter itemName  the name of the item
    || @parameter lvl  the level of this item in the hierachy
    || @parameter shortKey the mnemonic 'shortkey'
    */
    MenuItem(MenuBackend &mb, const char* itemName, unsigned char lvl = 0, char shortKey = '\0') : name(itemName), shortkey(shortKey)
    {
      registerBackend(mb);
      before = right = after = left = 0;
      value = 0;
      level = lvl;
    }

    /*
    || @description
    || | Register a backend for this item to use for callbacks and such
    || #
    ||
    || @parameter mb the menu backend that controls this item
    */
    inline void registerBackend(MenuBackend &mb)
    {
      menuBackend = &mb;
    }

    //void use(){} //update some internal data / statistics
    /*
    || @description
    || | Get the name of this item
    || #
    ||
    || @return The name of this item
    */
    inline const char* getName() const
    {
      return name;
    }
    /*
    || @description
    || | Get the shortkey of this item
    || #
    ||
    || @return The shortkey of this item
    */
    inline const char getShortkey() const
    {
      return shortkey;
    }
    /*
    || @description
    || | Check to see if this item has a shorkey
    || #
    ||
    || @return true if this item has a shorkey
    */
    inline const bool hasShortkey() const
    {
      return (shortkey != '\0');
    }
    /*
    || @description
    || | Get the level of this item
    || #
    ||
    || @return The level of this item
    */
    inline const unsigned char getLevel() const
    {
      return level;
    }
    /*
    || @description
    || | Set the level of this item
    || #
    ||
    || @parameter lvl the level of this item
    */
    inline void setLevel(unsigned char lvl)
    {
      level = lvl;
    }
    /*
    || @description
    || | Check to see if this item has children
    || #
    ||
    || @return true if the item has children
    */
    inline const bool hasChildren() const
    {
      return (before || right || after || left);
    }
    /*
    || @description
    || | Set the item that belongs as the historical 'back' for this item
    || #
    ||
    || @parameter b the item that was previously visited
    */
    inline void setBack(MenuItem *b)
    {
      back = b;
    }
    /*
    || @description
    || | Get the item 'back' of this item (historical back)
    || #
    ||
    || @return the item back from this
    */
    inline MenuItem *getBack() const
    {
      return back;
    }
    /*
    || @description
    || | Get the item 'before' this item
    || #
    ||
    || @return the item before this
    */
    inline MenuItem *getBefore() const
    {
      return before;
    }
    /*
    || @description
    || | Get the item 'right' of this item
    || #
    ||
    || @return the item right of this
    */
    inline MenuItem *getRight() const
    {
      return right;
    }
    /*
    || @description
    || | Get the item 'after' this item
    || #
    ||
    || @return the item after this
    */
    inline MenuItem *getAfter() const
    {
      return after;
    }
    /*
    || @description
    || | Get the item 'left' of this item
    || #
    ||
    || @return the item left of this
    */
    inline MenuItem *getLeft() const
    {
      return left;
    }
    /*
    || @description
    || | Get the item that represents the 'exit' of this item
    || #
    ||
    || @return the item that represents the exit node of this node
    */
    inline MenuItem *getExit() const
    {
      return exit;
    }
    //default vertical menu
    /*
    || @description
    || | Add an item after this item in the hierarchy
    || #
    ||
    || @parameter mi the item to add
    || @return the item sent as parameter for chaining
    */
    MenuItem &add(MenuItem &mi)
    {
      return addAfter(mi);
    }

    /*
    || @description
    || | Add an item before this item in the hierarchy
    || #
    ||
    || @parameter mi the item to add
    || @return the item sent as parameter for chaining
    */
    MenuItem &addBefore(MenuItem &mi)
    {
      mi.after = this;
      before = &mi;
      if (!mi.back) mi.back = back;
      return mi;
    }
    /*
    || @description
    || | Add an item right to this item in the hierarchy
    || #
    ||
    || @parameter mi the item to add
    || @return the item sent as parameter for chaining
    */
    MenuItem &addRight(MenuItem &mi)
    {
      mi.left = this;
      right = &mi;
      if (!mi.back) mi.back = back;
      return mi;
    }
    /*
    || @description
    || | Add an item after this item in the hierarchy
    || #
    ||
    || @parameter mi the item to add
    || @return the item sent as parameter for chaining
    */
    MenuItem &addAfter(MenuItem &mi)
    {
      mi.before = this;
      after = &mi;
      if (!mi.back) mi.back = back;
      return mi;
    }
    /*
    || @description
    || | Add an item left of this item in the hierarchy
    || #
    ||
    || @parameter mi the item to add
    || @return the item sent as parameter for chaining
    */
    MenuItem &addLeft(MenuItem &mi)
    {
      mi.right = this;
      left = &mi;
      if (!mi.back) mi.back = back;
      return mi;
    }
	/*
    || @description
    || | Add an item that represents exit node of this item in the hierarchy
    || #
    ||
    || @parameter ext is the exit item
    || @return the item sent as parameter for chaining
    */
    MenuItem &addExit(MenuItem &ext)
    {
      exit = &ext;
      return ext;
    }
	  
    /*
    || @description
    || | Set a callback to be fired before any 'move' function is called with this item 
    || | as the current MenuItem.  
    || #
    || 
    || @paramter cb The callback to be fired
    || @return this MenuItem
    */
    MenuItem &onChangeFrom(cb_change cb)
    {
      cb_onChangeFrom = cb;
      return *this;
    }
     /*
    || @description
    || | Set a callback to be fired after any 'move' function is called with this item 
    || | as the resulting MenuItem.  
    || #
    || 
    || @paramter cb The callback to be fired
    || @return this MenuItem
    */
    MenuItem &onChangeTo(cb_change cb)
    {
      cb_onChangeTo = cb;
      return *this;
    }
 
   /*
    || @description
    || | Set a callback to be fired when 'moveUp' is called with this item as the current MenuItem
    || #
    || 
    || @paramter cb The callback to be fired
    || @return this MenuItem
    */
    MenuItem &onUp(cb_move cb)
    {
      cb_onUp = cb;
      return *this;
    }
    /*
    || @description
    || | Set a callback to be fired when 'moveDown' is called with this item as the current MenuItem
    || #
    || 
    || @paramter cb The callback to be fired
    || @return this MenuItem
    */
    MenuItem &onDown(cb_move cb)
    {
      cb_onDown = cb;
      return *this;
    }
    /*
    || @description
    || | Set a callback to be fired when 'moveLeft' is called with this item as the current MenuItem
    || #
    || 
    || @paramter cb The callback to be fired
    || @return this MenuItem
    */
    MenuItem &onLeft(cb_move cb)
    {
      cb_onLeft = cb;
      return *this;
    }
    /*
    || @description
    || | Set a callback to be fired when 'moveRight' is called with this item as the current MenuItem
    || #
    || 
    || @paramter cb The callback to be fired
    || @return this MenuItem
    */
    MenuItem &onRight(cb_move cb)
    {
      cb_onRight = cb;
      return *this;
    }
    /*
    || @description
    || | Set a callback to be fired when 'use' is called with this item as the current MenuItem
    || #
    || 
    || @paramter cb The callback to be fired
    || @return this MenuItem
    */
    MenuItem &onUse(cb_use cb)
    {
      cb_onUse = cb;
      return *this;
    }
    //manipulate the value
    /*
    || @description
    || | Get the value for this item
    || #
    ||
    || @return the value
    */
    inline int getValue() const
    {
      return value;
    }
    /*
    || @description
    || | Set the value for this item
    || #
    ||
    || @parameter v the new value
    */
    inline void setValue(int v);   //placed in cpp because of the use of the menu backend
    /*
    || @description
    || | Increment the value of this item
    || #
    ||
    || @parameter v increment by v
    || @return the value
    */
    int increment(int v = 1);
    /*
    || @description
    || | Decrement the value of this item
    || #
    ||
    || @parameter v decrement by v
    || @return the value
    */
    int decrement(int v = 1);
    /*
    || @description
    || | Check to see if an item is equal to this
    || #
    ||
    || @parameter mi the item to check against this item
    || @return true if they have the same name
    */
    bool isEqual(MenuItem &mi)
    {
      //TODO extend equality test
      return (menuTestStrings(getName(), mi.getName()) && getValue() == mi.getValue());
    }

  protected:
    const char* name;
    const char shortkey;
    int value;
    unsigned char level;

    MenuItem *before;
    MenuItem *right;
    MenuItem *after;
    MenuItem *left;
    MenuItem *back;
	MenuItem *exit;

    cb_change cb_onChangeFrom;
    cb_change cb_onChangeTo;
    cb_move cb_onBack;
    cb_move cb_onUp;
    cb_move cb_onDown;
    cb_move cb_onLeft;
    cb_move cb_onRight;
    cb_use cb_onUse;

    MenuBackend *menuBackend;

  private:
    friend class MenuBackend;
    MenuItem *moveBack()
    {
      return back;
    }

    MenuItem *moveExit()
    {
      if (exit)
      {
        exit->back = this;
      }
      return back;
    }

    MenuItem *moveUp()
    {
      if (before)
      {
        before->back = this;
      }
      return before;
    }

    MenuItem *moveDown()
    {
      if (after)
      {
        after->back = this;
      }
      return after;
    }

    MenuItem *moveLeft()
    {
      if (left)
      {
        left->back = this;
      }
      return left;
    }

    MenuItem *moveRight()
    {
      if (right)
      {
        right->back = this;
      }
      return right;
    }
    //no dependant inclusion of string or cstring
    bool menuTestStrings(const char *a, const char *b)
    {
      while (*a)
      {
        if (*a != *b)
        {
          return false;
        }
        b++;
        a++;
      }
      return true;
    }
};

class MenuBackend
{
  public:
    MenuBackend(cb_use menuUse, cb_change menuChange = 0, cb_item_change itemChange = 0) : root("MenuRoot")
    {
      current = &root;
      cb_menuChange = menuChange;
      cb_menuUse = menuUse;
      cb_itemChange = itemChange;
    }

    /*
    || @description
    || | Get the root of this menu
    || #
    ||
    || @return the root item
    */
    MenuItem &getRoot()
    {
      return root;
    }
    /*
    || @description
    || | Get the current item of this menu
    || #
    ||
    || @return the current item
    */
    MenuItem &getCurrent()
    {
      return *current;
    }
    /*
    || @description
    || | Move back in the menu structure, will fire move event
    || #
    */
    void moveBack()
    {
      if (current->cb_onBack)
      {
        MenuMoveEvent mme = { *current };
        (*current->cb_onBack)(mme);
      }
      setCurrent(current->getBack());
    }
	/*
    || @description
    || | Move to the exit item for this node in the menu structure, will fire move event
    || #
    */
    void moveExit()
    {
      setCurrent(current->getExit());
    }
    /*
    || @description
    || | Move up in the menu structure, will fire move event
    || #
    */
    void moveUp()
    {
      if (current->cb_onUp)
      {
        MenuMoveEvent mme = { *current };
        (*current->cb_onUp)(mme);
      }
      setCurrent(current->moveUp());
    }
    /*
    || @description
    || | Move down in the menu structure, will fire move event
    || #
    */
    void moveDown()
    {
      if (current->cb_onDown)
      {
        MenuMoveEvent mme = { *current };
        (*current->cb_onDown)(mme);
      }
      setCurrent(current->moveDown());
    }
    /*
    || @description
    || | Move left in the menu structure, will fire move event
    || #
    */
    void moveLeft()
    {
      if (current->cb_onLeft)
      {
        MenuMoveEvent mme = { *current };
        (*current->cb_onLeft)(mme);
      }
      setCurrent(current->moveLeft());
    }
    /*
    || @description
    || | Move right in the menu structure, will fire move event
    || #
    */
    void moveRight()
    {
      if (current->cb_onRight)
      {
        MenuMoveEvent mme = { *current };
        (*current->cb_onRight)(mme);
      }
      setCurrent(current->moveRight());
    }
    /*
    || @description
    || | Use an item
    || #
    ||
    || @parameter item is the item to use
    */
    void use(MenuItem &item)
    {
      setCurrent(&item);
      use();
    }
    /*
    || @description
    || | Use an item per its shortkey
    || #
    ||
    || @parameter shortkey the shortkey of the target item
    */
    void use(char shortkey)
    {
      recursiveSearch(shortkey, &root);
      use();
    }
    /*
    || @description
    || | Use an item, will fire use event
    || #
    */
    void use()
    {
      //current->use();
      if (current->cb_onUse)
      {
        MenuUseEvent mue = { *current };
        (*current->cb_onUse)(mue);
      }
      if (cb_menuUse)
      {
        MenuUseEvent mue = { *current };
        cb_menuUse(mue);
      }
    }
    /*
    || @description
    || | Select an item, will fire change event
    || #
    */
    void select(MenuItem &item)
    {
      setCurrent(&item);
    }
    /*
    || @description
    || | Move relatively in the menu hierarchy level
    || #
    ||
    || @parameter levels number of levels to move
    */
    void moveRelativeLevels(int levels)
    {
      int move = levels + current->getLevel();
      moveToLevel(move);
    }
    /*
    || @description
    || | Move to a specific menu hierarchy level
    || #
    ||
    || @parameter level the target level to go to
    */
    void moveToLevel(unsigned char level)
    {
      MenuItem *cur = current;
      recursiveSearchForLevel(level, current);
      if (cur != current)
      {
        MenuChangeEvent mce = { *cur, *current };
        if (cur->cb_onChangeFrom)
        {
          (*cur->cb_onChangeFrom)(mce);
        }
        if (current->cb_onChangeTo)
        {
          (*current->cb_onChangeTo)(mce);
        }
        if (cb_menuChange)
        {
          (*cb_menuChange)(mce);
        }
      }
    }
    /*
    || @description
    || | Fire a callback for change
    || #
    ||
    || @parameter mi the item that changed
    */
    void fireItemChangedEvent(MenuItem *mi)
    {
      if (cb_itemChange)
      {
        MenuItemChangeEvent mice = { *mi };
        (*cb_itemChange)(mice);
      }
    }

  private:
    void setCurrent(MenuItem *next)
    {
      if (next)
      {
        MenuChangeEvent mce = { *current, *next };
        if (current->cb_onChangeFrom)
        {
          (*current->cb_onChangeFrom)(mce);
        }
        if (next->cb_onChangeTo)
        {
          (*next->cb_onChangeTo)(mce);
        }
        if (cb_menuChange)
        {
          (*cb_menuChange)(mce);
        }
        current = next;
      }
    }

    void foundShortkeyItem(MenuItem *mi)
    {
      mi->setBack(current);
      current = mi;
    }

    char canSearch(const char shortkey, MenuItem *m)
    {
      if (m == 0)
      {
        return 0;
      }
      else
      {
        if (m->getShortkey() == shortkey)
        {
          foundShortkeyItem(m);
          return 1;
        }
        return -1;
      }
    }

    void rSAfter(const char shortkey, MenuItem *m)
    {
      if (canSearch(shortkey, m) != 1)
      {
        rSAfter(shortkey, m->getAfter());
        rSRight(shortkey, m->getRight());
        rSLeft(shortkey, m->getLeft());
      }
    }

    void rSRight(const char shortkey, MenuItem *m)
    {
      if (canSearch(shortkey, m) != 1)
      {
        rSAfter(shortkey, m->getAfter());
        rSRight(shortkey, m->getRight());
        rSBefore(shortkey, m->getBefore());
      }
    }

    void rSLeft(const char shortkey, MenuItem *m)
    {
      if (canSearch(shortkey, m) != 1)
      {
        rSAfter(shortkey, m->getAfter());
        rSLeft(shortkey, m->getLeft());
        rSBefore(shortkey, m->getBefore());
      }
    }

    void rSBefore(const char shortkey, MenuItem *m)
    {
      if (canSearch(shortkey, m) != 1)
      {
        rSRight(shortkey, m->getRight());
        rSLeft(shortkey, m->getLeft());
        rSBefore(shortkey, m->getBefore());
      }
    }

    void recursiveSearch(const char shortkey, MenuItem *m)
    {
      if (canSearch(shortkey, m) != 1)
      {
        rSAfter(shortkey, m->getAfter());
        rSRight(shortkey, m->getRight());
        rSLeft(shortkey, m->getLeft());
        rSBefore(shortkey, m->getBefore());
      }
    }

    void rSLAfter(const unsigned char level, MenuItem *m)
    {
      if (m)
      {
        if (m->getLevel() == level)
        {
          setCurrent(m);
        }
        else
        {
          rSLAfter(level, m->getAfter());
          rSLRight(level, m->getRight());
          rSLLeft(level, m->getLeft());
        }
      }
    }
    void rSLRight(const unsigned char level, MenuItem *m)
    {
      if (m)
      {
        if (m->getLevel() == level)
        {
          setCurrent(m);
        }
        else
        {
          rSLAfter(level, m->getAfter());
          rSLRight(level, m->getRight());
          rSLBefore(level, m->getBefore());
        }
      }
    }
    void rSLLeft(const unsigned char level, MenuItem *m)
    {
      if (m)
      {
        if (m->getLevel() == level)
        {
          setCurrent(m);
        }
        else
        {
          rSLAfter(level, m->getAfter());
          rSLLeft(level, m->getLeft());
          rSLBefore(level, m->getBefore());
        }
      }
    }
    void rSLBefore(const unsigned char level, MenuItem *m)
    {
      if (m)
      {
        if (m->getLevel() == level)
        {
          setCurrent(m);
        }
        else
        {
          rSLRight(level, m->getRight());
          rSLLeft(level, m->getLeft());
          rSLBefore(level, m->getBefore());
        }
      }
    }
    void recursiveSearchForLevel(const unsigned char level, MenuItem *m)
    {
      if (m)
      {
        rSLAfter(level, m->getAfter());
        rSLRight(level, m->getRight());
        rSLLeft(level, m->getLeft());
        rSLBefore(level, m->getBefore());
      }
    }

    MenuItem root;
    MenuItem *current;

    cb_change cb_menuChange;
    cb_use cb_menuUse;
    cb_item_change cb_itemChange;
};

#endif
// MENUBACKEND_H