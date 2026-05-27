import { createBrowserRouter } from "react-router";
import Home from "./pages/home"
import Sandbox from "./pages/Sandbox"
import Layout from "./layout"
import StepsRenderer from "./components/StepsRenderer";

const router = createBrowserRouter([
  {
    path: "/",
    Component: Layout,
    children: [
      { index: true, Component: Sandbox},
    ]
  }
]);

export default router;