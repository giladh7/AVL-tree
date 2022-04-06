#ifndef _GENERICAVL_H_
#define _GENERICAVL_H_

template<class T>
class AVL {
  /**
   * To manage the tree nodes, we use a nested struct. This struct contains the
   * T corresponding to the node, the left son and the right son of the
   * node, both of them node type themselves.
   */
 public:
  class node {
   public:
    /**
     * Constructor
     * @param data the corresponding T object
     * @param left child
     * @param right child
     */
    node (T data, node *left, node *right, node *parent, int height)
        : data_ (data), left_ (left), right_ (right), parent_ (parent),
          height_ (height)
    {
      balance = 0;
    }

    /**
    *
    * @return the left child of this node
    */
    node *get_left () const
    {
      return left_;
    }
    /**
     *
     * @return the right child of this node
     */
    node *get_right () const
    {
      return right_;
    }

    /**
   *
   * @return the const reference T of this node
   */
    const T &get_data () const
    {
      return data_;
    }
    node *left_, *right_, *parent_;
    int height_, balance;
   public:
    /**
      * node destructor. delete its successors also
      */
  ~node ()
  {
    delete right_;
    delete left_;
  }
      T data_;
  };
 public:
  /**
   * Constructor. Constructs an empty AVL tree
   */
  AVL ()
  {
    root_ = nullptr;
  }
  /**
   * Copy constructor
   * @param other
   */
  AVL (const AVL &other)
  {
    node *new_root = new node (other.root_->data_, nullptr, nullptr, nullptr, 0);
    copy_sub_tree (new_root, other.root_);
    root_ = new_root;
  }

  /**
  * Destructor.
  */
  ~AVL ()
  {
    delete root_;
  }

  /**
 * assignment operator
 * @param rhs
 * @return
 */
  AVL &operator= (const AVL &rhs)
  {
    if (this == &rhs) return *this;
    delete root_;
    node *new_root = new node (rhs.root_->data_, nullptr, nullptr, nullptr, 0);
    copy_sub_tree (new_root, rhs.root_);
    root_ = new_root;
    return *this;
  }

  /**
   *
   * @return the root node of this tree
   */
  node *get_root () const
  {
    return root_;
  }

  /**
   * The function inserts the new T into the tree so that it maintains
   * the legality of the tree.
   * @param T
   */
  void insert (const T & val)
  {
    node *new_node = new node (val, nullptr, nullptr, nullptr, 0);
    node_insert (new_node);
    balance_up (new_node);
  }
  /**
   * The function deletes the T from the tree (if it is in that tree)
   * so that it maintains the legality of the tree.
   * @param T
   */
  void erase (const T &val)
  {
    // finds the node to erase
    AVL::node *node = find_node (val);
    if (node == nullptr) return;

    AVL::node *balance_from = node->parent_;
    // if it has the most one child - replace with the other
    if (node->left_ == nullptr) transplant (node, node->right_);
    else if (node->right_ == nullptr) transplant (node, node->left_);
    else // the node has two children - replace with the successor
      {
        AVL::node *successor = subtree_minimum (node->right_);
        balance_from = successor->parent_;
        if (successor->parent_ != node)
          {
            transplant (successor, successor->right_);
            successor->right_ = node->right_;
            successor->right_->parent_ = successor;
          }
        transplant (node, successor);
        successor->left_ = node->left_;
        successor->left_->parent_ = successor;
      }
    balance_up (balance_from);
    node->left_ = nullptr;
    node->right_ = nullptr;
    delete node;
  }

  /**
   * The iterator will move in preorder.
   */
  class ConstIterator;
  class Iterator;
  class Iterator {
    node *cur;
    friend class ConstIterator;
   public:
    /* iterator traits */
    typedef T value_type;
    typedef T &reference;
    typedef T *pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;

    Iterator (node *cur) : cur (cur)
    {}

    pointer operator-> ()
    {
      return &cur->data_;
    }
    reference operator* ()
    {
      return cur->data_;
    }
    Iterator &operator++ ()
    {
      if (cur->left_ != nullptr) cur = cur->left_;
      else if (cur->right_ != nullptr) cur = cur->right_;
        // probing up until the first ancestor which is left child of his parent
      else preorder_probing ();
      return *this;
    }
    Iterator operator++ (int)
    {
      iterator it = *this;
      if (cur->left_ != nullptr) cur = cur->left_;
      else if (cur->right_ != nullptr) cur = cur->right_;

        // probing up until the first ancestor which is left child of his parent
      else preorder_probing ();
      return it;
    }
    bool operator== (const Iterator &rhs) const
    {
      return cur == rhs.cur;
    }
    bool operator!= (const Iterator &rhs) const
    {
      return !(rhs == *this);
    }

    /**
    * finds the next preorder node for a node without childrens.
    * update cur to that node
    */
    void preorder_probing ()
    {
      node *traveler = cur->parent_;
      // probing to the first ancestor with right child which isn't ancestor
      while (traveler != nullptr && (traveler->right_ == nullptr
                                     || traveler->right_ == cur))
        {
          cur = cur->parent_;
          traveler = traveler->parent_;
        }
      cur = (traveler != nullptr) ? traveler->right_ : traveler;
    }
  };

  class ConstIterator {
    node *cur;
   public:
    /* iterator traits */
    typedef const T value_type;
    typedef const T &reference;
    typedef const T *pointer;
    typedef std::forward_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;

    ConstIterator (node *cur) : cur (cur)
    {}
    ConstIterator (const Iterator &it) : cur (it.cur)
    {}

    pointer operator-> () const
    {
      return &cur->data_;
    }
    reference operator* () const
    {
      return cur->data_;
    }
    ConstIterator &operator++ ()
    {
      if (cur->left_ != nullptr) cur = cur->left_;
      else if (cur->right_ != nullptr) cur = cur->right_;
        // probing up until the first ancestor which is left child of his parent
      else preorder_probing ();
      return *this;
    }
    ConstIterator operator++ (int)
    {
      ConstIterator it = *this;
      if (cur->left_ != nullptr) cur = cur->left_;
      else if (cur->right_ != nullptr) cur = cur->right_;

        // probing up until the first ancestor which is left child of his parent
      else preorder_probing ();
      return it;
    }
    bool operator== (const ConstIterator &rhs) const
    {
      return cur == rhs.cur;
    }
    bool operator!= (const ConstIterator &rhs) const
    {
      return !(rhs == *this);
    }

    /**
    * finds the next preorder node for a node without childrens.
    * update cur to that node
    */
    void preorder_probing ()
    {
      node *traveler = cur->parent_;
      // probing to the first ancestor with right child which isn't ancestor
      while (traveler != nullptr && (traveler->right_ == nullptr
                                     || traveler->right_ == cur))
        {
          cur = cur->parent_;
          traveler = traveler->parent_;
        }
      cur = (traveler != nullptr) ? traveler->right_ : traveler;
    }
  };

  using iterator = Iterator; // same as typedef Iterator iterator;
  using const_iterator = ConstIterator;

  iterator begin ()
  { return {root_}; }
  iterator end ()
  { return {nullptr}; }
  const_iterator begin () const
  { return {root_}; }
  const_iterator end () const
  { return {nullptr}; }
  const_iterator cbegin () const
  { return {root_}; }
  const_iterator cend () const
  { return {nullptr}; }


  /**
   * The function returns an iterator to the item that corresponds to the item
   * we were looking for. If there is no such member, returns end ().
   * @param data T to search
   * @return iterator to the item that corresponds to the item
   * we were looking for. If there is no such member, returns end ().
   */
  iterator find (const T &data)
  {
    AVL::node *node = find_node (data);
    if (node != nullptr) return {node};
    else return end ();
  }
  /**
   * The function returns an iterator to the item that corresponds to the item
   * we were looking for. If there is no such member, returns end ().
   * @param data T to search
   * @return iterator to the item that corresponds to the item
   * we were looking for. If there is no such member, returns end ().
   */
  const_iterator find (const T &data) const
  {
    AVL::node *node = find_node (data);
    if (node != nullptr) return {node};
    else return end ();
  }
  /**
   * Insertion operator, prints the T in the tree in preorder
   * traversal.
   * Each apartmnet will be printed in the format: (x,y)\n
   * @param os reference to std::ostream
   * @param avl tree
   * @return os reference to std::ostream
   */
  friend std::ostream &operator<< (std::ostream &os, const AVL &avl)
  {
    for (auto node: avl)
      {
        os << node;
        os << std::endl;
      }
    return os;
  }
 private:
  node *root_;

  /**
 * a general Binary Search Tree insert
 * @param node the node to insert to the AVL tree
 */
  void node_insert (node *node)
  {
    AVL::node *parent = nullptr;
    AVL::node *current = root_;

    // find where to add the new node
    while (current != nullptr)
      {
        parent = current;
        if (node->data_ < current->data_) current = current->left_;
        else current = current->right_;
      }

    // the inserting itself
    node->parent_ = parent;
    if (parent == nullptr) root_ = node; // tree was empty
    else if (node->data_ < parent->data_) parent->left_ = node;
    else parent->right_ = node;
  }
  /**
 * balancing and updating the balace factor along the tree from bottom up
 * @param node the node to start balance from
 */
  void balance_up (node *node)
  {
    while (node != nullptr)
      {
        // updating height
        update_height (node);
        // balancing if needed
        if (std::abs (node->balance) >= 2) rotate (node);
        node = node->parent_;
      }
  }
  /**
 * updating the tree node's height and balance factor)
 * @param node the node to update
 */
  static void update_height (node *node)
  {
    int left_h, right_h;
    left_h = (node->left_ == nullptr) ? -1 : node->left_->height_;
    right_h = (node->right_ == nullptr) ? -1 : node->right_->height_;
    node->height_ = std::max (left_h, right_h) + 1;
    node->balance = left_h - right_h;
  }
  /**
 * left rotate the sub-tree rooted in node
 * @param node
 */
  void left_rotation (node *node)
  {
    AVL::node *other_node = node->right_;  // the right child - the future parent
    other_node->parent_ = node->parent_;
    node->parent_ = other_node;
    node->right_ = other_node->left_;
    if (node->right_) node->right_->parent_ = node;
    other_node->left_ = node;

    // updating node->parent or the tree root
    update_parent (node, other_node);
    update_height (node);
    update_height (other_node);
  }
  /**
 * right rotate the sub-tree rooted in node
 * @param node
 */
  void right_rotation (node *node)
  {
    AVL::node *other_node = node->left_;  // the left child - the future parent
    other_node->parent_ = node->parent_;
    node->parent_ = other_node;
    node->left_ = other_node->right_;
    if (node->left_) node->left_->parent_ = node;
    other_node->right_ = node;

    update_parent (node, other_node);
    update_height (node);
    update_height (other_node);
  }
  /**
 *  rotate the sub-tree rooted in node
 * @param node
 */
  void rotate (node *node)
  {
    if (node->balance == 2)
      {
        if (node->left_ && node->left_->balance == -1)
          {
            // LR
            left_rotation (node->left_);
            right_rotation (node);
          }
        else
          {
            // LL
            right_rotation (node);
          }
      }
    else if (node->balance == -2)
      {
        if (node->right_ && node->right_->balance == 1)
          {
            // RL
            right_rotation (node->right_);
            left_rotation (node);
          }
        else
          {
            // RR
            left_rotation (node);
          }
      }
  }
  /**
 * after rotation - update the rotated node parent. if needed - update the
 * tree root
 * @param rotated_node
 * @param new_parent
 */
  void update_parent (const node *rotated_node, node *new_parent)
  {
    if (root_ == rotated_node) root_ = new_parent;
    else
      {
        if (rotated_node == new_parent->parent_->right_)
          {
            new_parent->parent_->right_ = new_parent;
          }
        else
          {
            new_parent->parent_->left_ = new_parent;
          }
      }
  }
  /**
 * recursively prints the nodes of the sub tree rooted in node
 * @param os
 * @param node
 */
  static void recursive_inorder_print (std::ostream &os, node *node)
  {
    if (node != nullptr)
      {
        os << node->data_;
        recursive_inorder_print (os, node->left_);
        recursive_inorder_print (os, node->right_);
      }
  }
  /**
 * Getting an T and return the node contains that T, or nullptr
 * if doesn't exist
 * @param data
 * @return *node to the node contains the data, or nullptr if doesn't exist
 */
  node *find_node (const T &data) const
  {
    node *traveler = root_;
    while (traveler != nullptr && !(traveler->data_ == data))
      {
        if (traveler->data_ < data) traveler = traveler->right_;
        else traveler = traveler->left_;
      }
    return traveler;
  }
  /**
 * Transplants the sub-tree rooted in transplanted_node into erased_node
 * @param erased_node
 * @param transplanted_node
 */
  void transplant (node *erased_node, node *transplanted_node)
  {
    if (erased_node->parent_ == nullptr) root_ = transplanted_node;
    else if (erased_node == erased_node->parent_->left_)
      {
        erased_node->parent_->left_ = transplanted_node;
      }
    else erased_node->parent_->right_ = transplanted_node;
    if (transplanted_node != nullptr)
      {
        transplanted_node->parent_ = erased_node->parent_;
      }
  }
  /**
 * returns the minimum of the tree rooted in node
 * @param node
 * @return a node* to the minimum
 */
  static node *subtree_minimum (node *node)
  {
    while (node->left_ != nullptr) node = node->left_;
    return node;
  }
  /**
 * Gets two nodes, and copied the tree in the old one, into the new one
 * @param new_node
 * @param old_node
 */
  void copy_sub_tree (node *new_node, node *old_node)
  {
    new_node->height_ = old_node->height_;
    new_node->balance = old_node->balance;
    if (old_node->left_ != nullptr)
      {
        auto *new_left = new node (old_node->left_->data_, nullptr,
                                   nullptr, new_node, 0);
        copy_sub_tree (new_left, old_node->left_);
        new_node->left_ = new_left;
      }
    else new_node->left_ = nullptr;

    if (old_node->right_ != nullptr)
      {
        auto *new_right = new node (old_node->right_->data_, nullptr,
                                    nullptr, new_node, 0);
        copy_sub_tree (new_right, old_node->right_);
        new_node->right_ = new_right;
      }
    else new_node->right_ = nullptr;
  }
};

#endif //_GENERICAVL_H_
