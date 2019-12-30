open Brisk_reconciler;

[@noalloc] external menuSupported: unit => bool = "revery_menuSupported";

type menu;

module UIDGenerator = {
  let v = ref(1);

  let gen = () => {
    let ret = v^;
    let () = incr(v);
    ret;
  };
};

type callback('a) = 'a => unit;
let noopCallback = () => ();

module MenuItem = {
  type menuItem =
    | Label(string, int, ref(option(menu)));

  let%nativeComponent make = (~label, ~callback as _=noopCallback, (), hooks) => (
    {
      make: () => Label(label, UIDGenerator.gen(), ref(None)),
      configureInstance: (~isFirstRender as _, obj) => obj,
      children: Brisk_reconciler.empty,
      insertNode: (~parent, ~child as _, ~position as _) => parent,
      deleteNode: (~parent, ~child as _, ~position as _) => parent,
      moveNode: (~parent, ~child as _, ~from as _, ~to_ as _) => parent,
    },
    hooks,
  );
};

external createMenu: unit => menu = "revery_create_menu";

external insertNode: (menu, int, int, string) => bool =
  "revery_menu_insert_node_string";

let%nativeComponent make =
                    (
                      ~children: Brisk_reconciler.element(MenuItem.menuItem),
                      (),
                      hooks,
                    ) => (
  {
    make: createMenu,
    configureInstance: (~isFirstRender as _, obj) => obj,
    children,
    insertNode: (~parent, ~child, ~position) => {
      let _: bool =
        switch (child) {
        | Label(child, uid, parent') =>
          parent' := Some(parent);
          insertNode(parent, position, uid, child);
        };
      parent;
    },
    deleteNode: (~parent, ~child as _, ~position as _) => parent,
    moveNode: (~parent, ~child as _, ~from as _, ~to_ as _) => parent,
  },
  hooks,
);