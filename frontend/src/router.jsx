import { createBrowserRouter } from "react-router";
import Home from "./pages/home"
import Sandbox from "./pages/Sandbox"
import Layout from "./layout"

const router = createBrowserRouter([
  {
    path: "/",
    Component: Layout,
    children: [
      { index: true, Component: Home},
      { path:"/sandbox", Component:Sandbox}
    ]
  }
]);

export default router;